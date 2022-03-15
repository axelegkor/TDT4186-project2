#include <stdio.h>
#include "sem.h"
#include "bbuffer.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>

#define PORT 8000;

void communication(int connection)
{
    
}

int main() {
    
    int mainSocket;

    mainSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mainSocket == -1) {
        printf("Socket failed to be created\n");
        exit(0);
    }
    printf("Socket was successfully created!\n");

    struct sockaddr_un my_addr, peer_addr;
    socklen_t peer_addr_size;

    memset(&my_addr, 0, sizeof(struct sockaddr_un));
    
    //my_addr.sun_family = AF_UNIX;
    //strncpy(my_addr.sun_path, MY_SOCK_PATH, sizeof(my_addr.sun_path) - 1);


    int binder;
    binder = bind(mainSocket, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_un));
    if (binder == -1){
        printf("Socket failed to be bound\n");
        exit(0);
    }
    printf("Socket was successfully bound!\n");

    int listener;
    listener = listen(mainSocket, 128);
    if (listener == -1){
        printf("Socket failed to listen\n");
        exit(0);
    }
    printf("Socket was successfully set as passive (listening)!\n");

    peer_addr_size = sizeof(struct sockaddr_un);

    int descriptor_accepted;
    descriptor_accepted = accept(mainSocket, (struct sockaddr *) &peer_addr, &peer_addr_size);
    if (descriptor_accepted == -1){
        printf("Failed to accept incoming connection\n");
        exit(0);
    }
    printf("Connection %d was accepted!", descriptor_accepted);

    communication(descriptor_accepted);

    close(mainSocket);

} 