#include <stdio.h>
#include "sem.h"
#include "bbuffer.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>


#define PORT 6969
#define MAXREQ (4096*1024)

char buffer[MAXREQ], body[MAXREQ], msg[MAXREQ];

int main() {

    int mainSocket, newSocket;
    struct sockaddr_in serverAddress, clientAddress;

    mainSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (mainSocket == -1) {
        printf("Could not creat socket\n");
        exit(0);
    }
    bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    int binder;
    binder = bind(mainSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (binder == -1) {
        printf("Could not bind\n");
        exit(0);
    }
    //bzero((char *) &serverAddress, sizeof(serverAddress));


    int listener;
    listen(mainSocket, 128);
    if (listener == -1) {
        printf("Could not listen to socket\n");
        exit(0);
    }

    socklen_t clientLength;
    int n;

    while (1)
    {
        clientLength = sizeof(clientAddress);

        newSocket = accept(mainSocket, (struct sockaddr *) &clientAddress, &clientLength);
        if (newSocket == -1) {
            printf("Could not socket\n");
            exit(0);
        }
        bzero(buffer, sizeof(buffer));


        n = read(newSocket, buffer, sizeof(buffer) - 1);
        if (n == -1) {
            printf("Could read from socket\n");
            exit(0);
        }

        snprintf (body, sizeof(body),
            "<html>\n<body>\n"
            "<h1>Hello web browser</h1>\nYour request was\n"
            "<pre>%s</pre>\n"
            "</body>\n</html>\n", buffer);

        snprintf (msg, sizeof(msg),
            "HTTP/1.0 200 OK\n"
            "Content-Type: text/html\n"
            "Content-Length: %d\n\n%s"
            , strlen(body), body);

        n = write(newSocket, msg, strlen(msg));
        if (n == -1) {
            printf("Could write to socket\n");
            exit(0);
        }

        close(newSocket);
    }

    return 0;
} 