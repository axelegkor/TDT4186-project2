#include <stdio.h>
#include "sem.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "bbuffer.h"

#define MAXREQ (4096*1024)

BNDBUF *bbuffer;
char buffer[MAXREQ], body[MAXREQ], msg[MAXREQ];
FILE *fileOpner;
int threads, bufferSlots, portnumber;
char www_path[MAXREQ], path[1024];

void handle_connection(int fileDesc, int threadId) {
    int saved;
    printf("Thread id: %d\n", threadId);

    bzero(buffer, sizeof(buffer));

    saved = read(fileDesc, buffer, sizeof(buffer) - 1);
    if (saved == -1) {
        printf("Failed to read from socket");
    }

    // Finding the right path
    strcpy(path, www_path);
    char * s;
    s = strtok(buffer, " ");
    s = strtok(NULL, " ");
    strcat(path, s);

    fileOpner = fopen(path, "r");
    printf("The finale path: %s\n", path);

    if (fileOpner == NULL) {
        fileOpner = fopen("404page.html", "r");
    }
    if (fileOpner != NULL) {
        size_t length = fread(body, sizeof(char), MAXREQ, fileOpner);
        if (ferror(fileOpner) != 0) {
            fputs("An error occured when reading file\n", stderr);
        } else {
            body[length++] = '\0';
        }
    }
    fclose(fileOpner);

    snprintf (msg, sizeof (msg),
    "HTTP/1.0 200 OK\n"
    "Content-Type: text/html\n"
    "Content-Length: %d\n\n%s"
    , strlen (body), body);

    saved = write(fileDesc, msg, strlen(msg));
    if (saved == -1) {
        printf("Failed in writing to socket");
    }
    close(fileDesc);
}

void * thread_function(int *threadId) {
    while(1) {
        int file_descriptor = bb_get(bbuffer);
        //We have a connection
        if(file_descriptor != 0) {
            handle_connection(file_descriptor, threadId);
        }
    }
}

int main(int argc, char *argv[]) {
    
    int mainSocket;
    struct sockaddr_in6 serverAddress;
    
    if (argv[1]) {
        strcpy(www_path, argv[1]);
        printf("Serving the path: %s\n", www_path);
    }
    else {
        //printf("Did not find the www_path.\n");
        exit(0);
    } 
    if (argv[2]) {
        portnumber = atoi(argv[2]);
        printf("Portnumber: %d\n", portnumber);

    }
    if (argv[3]) {
        threads = atoi(argv[3]);
        printf("Threds: %d\n", threads);

    }

    bufferSlots = atoi(argv[4]);
    printf("Bufferslots: %d\n", bufferSlots);

    pthread_t thread_pool[threads];
    bbuffer = bb_init(bufferSlots);
    for(int i = 0;i< threads;i++) {
        pthread_create(&thread_pool[i], NULL, thread_function, (void *)i);
    }

    mainSocket = socket(AF_INET6, SOCK_STREAM, 0);
    if (mainSocket == -1) {
        printf("Socket failed to be created\n");
        exit(0);
    }
    printf("The socket was successfully created!\n");
    
    const int one = 1;
    if ((setsockopt(mainSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof(one))) < 0) {
        printf("Failed to reuse port");
        exit(0);
    }

    bzero(&serverAddress, sizeof(serverAddress));

    serverAddress.sin6_family = AF_INET6;
    serverAddress.sin6_addr = in6addr_any; // htonl(INADDR_ANY);
    serverAddress.sin6_port = htons(portnumber);

    if (bind(mainSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1){
        printf("Socket failed to be bound\n");
        exit(0);
    }
    printf("Socket was successfully bound!\n");

    // bzero(&serverAddress, sizeof(serverAddress));

    if (listen(mainSocket, 128) == -1){
        printf("Socket failed to listen\n");
        exit(0);
    }
    printf("Socket was successfully set as passive (listening)!\n");

    struct sockaddr_in clientAddress;
    socklen_t clientLenght;
    int newSocket;

    while(1) {
        clientLenght = sizeof(clientAddress);
        newSocket = accept(mainSocket, (struct sockaddr *) &clientAddress, &clientLenght);
        if (newSocket == -1) {
            printf("Failed to accept incoming connection\n");
            exit(0);
        }

        printf("Connection was accepted.\n");
        bb_add(bbuffer, newSocket);
    }
    return 0;
    
}