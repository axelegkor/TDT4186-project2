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
#define THREAD_POOL_SIZE 20

pthread_t thread_pool[THREAD_POOL_SIZE];
BNDBUF *bbuffer;
char buffer[MAXREQ], body[MAXREQ], msg[MAXREQ];
FILE *fileOpner;
int threads, bufferSlots, portnumber;
char www_path[MAXREQ], path[1024];

void * thread_function(int argc, char *argv[]) {
    while(1) {
        int file_descriptor = bb_get(bbuffer);

        //We have a connection
        if(file_descriptor != 0) {
            handle_connection(file_descriptor);
        }
    }
}

void handle_connection(int *p_file_descriptor) {
    int newSocket = *p_file_descriptor;
    int saved;
    free(*p_file_descriptor);

    bzero(buffer, sizeof(buffer));

    saved = read(newSocket, buffer, sizeof(buffer) - 1);
    if (saved == -1) {
        printf("Failed to read from socket");
    }

    // Finding the right path
    strcpy(path, www_path);
    strtok(buffer, " ");
    strcat(path, strtok(NULL, " "));

    fileOpner = fopen(path, "r");
    printf("The finale path: %s\n", path);

    if (fileOpner == NULL) 
            fileOpner = fopen("404page.html", "r");

    if (fileOpner != NULL) {
    size_t newLen = fread(body, sizeof(char), MAXREQ, fileOpner);
    if ( ferror( fileOpner ) != 0 ) {
        fputs("Error reading file", stderr);
    } else {
        body[newLen++] = '\0'; /* Just to be safe. */
    }
    fclose(fileOpner);
    }
    // if (s != NULL) {
    //     path = malloc(strlen(www_path) + strlen(s) + 1);
    //     strcpy(path, www_path);
    //     strcpy(path, s);
    // }
    // read_file(path, body);

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

int main(int argc, char *argv[]) {
    
    BNDBUF *bbuffer = bb_init(20);
    bb_get(bbuffer);
}