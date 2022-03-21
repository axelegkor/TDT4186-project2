#include <stdio.h>
#include "sem.h"
#include "bbuffer.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "bbuffer.h"
#include "bbuffer.c"

#define PORT 8000
#define MAXREQ (4096*1024)
#define THREAD_POOL_SIZE 20

pthread_t thread_pool[THREAD_POOL_SIZE];
BNDBUF *bbuffer;
char buffer[MAXREQ], body[MAXREQ], msg[MAXREQ];

void * thread_function(void *arg) {
    while(1) {
        int file_descriptor = bb_get(bbuffer);

        //We have a connection
        if(file_descriptor != 0) {
            handle_connection(file_descriptor);
        }
    }
}

void * handle_connection(int *p_socket) {
    int saved;
    int newSocket = *p_socket;
    free(*p_socket);
    bzero(buffer, sizeof(buffer));
    saved = read(newSocket, buffer, sizeof(buffer) - 1);
    if (saved == -1) {
        printf("Failed to read from socket");
    }
    snprintf (body, sizeof (body),
    "<html>\n<body>\n <h1>Hello web browser</h1>\nYour request was\n <pre>%s</pre>\n </body>\n</html>\n", buffer);

    snprintf (msg, sizeof (msg),
    "HTTP/1.0 200 OK\n"
    "Content-Type: text/html\n"
    "Content-Length: %d\n\n%s"
    , strlen (body), body);

    saved = write(newSocket, msg, strlen(msg));
    if (saved == -1) {
        printf("Failed in writing to socket");
    }
    close(newSocket);
}

int main() {
    
    int mainSocket; 
    bbuffer = bb_init(THREAD_POOL_SIZE);
    for(int i = 0;i< THREAD_POOL_SIZE;i++) {
        pthread_create(&thread_pool[i], NULL, thread_function, NULL);
    }

    mainSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (mainSocket == -1) {
        printf("Socket failed to be created\n");
        exit(0);
    }
    printf("Socket was successfully created!\n");

    struct sockaddr_in serverAddress;

    bzero(&serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(PORT);

    if (bind(mainSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1){
        printf("Socket failed to be bound\n");
        exit(0);
    }
    printf("Socket was successfully bound!\n");

    bzero(&serverAddress, sizeof(serverAddress));

    if (listen(mainSocket, 128) == -1){
        printf("Socket failed to listen\n");
        exit(0);
    }
    printf("Socket was successfully set as passive (listening)!\n");

    struct sockaddr_in clientAddress;
    socklen_t clientLenght;
    int newSocket;
    int saved;

    while(1) {
        clientLenght = sizeof(clientAddress);
        newSocket = accept(mainSocket, (struct sockaddr *) &clientAddress, &clientLenght);
        if (newSocket == -1) {
            printf("Failed to accept incoming connection\n");
            exit(0);
        }

        bb_add(bbuffer, newSocket);
    }
}