/*
This code is pretty mutch  a copy of Brian Jorgensens server program in
    "Beej's Guide to Network Programming Using Internet Sockets"
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BACKLOG 10
#define MAXBUFF 1024

void *get_in_addr(struct sockaddr *sa);

int main(int argc, char **argv){

    fd_set master;              // Master file descriptor list
    fd_set readfds;             // Temporary file descriptor list
    int fdmax;                  // maximum file descriptor number

    int listener, newsocket;    // Our Sockets
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;

    char buffer[MAXBUFF];       // Buffer to store client messages
    int bytes;

    char remoteIP[INET6_ADDRSTRLEN];   // Used for storing the remote IP of a socket

    int yes=1;
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    if(argc != 2){
        printf("Usage: %s <bind_port>\n", argv[0]);
        exit(-1);
    }

    FD_ZERO(&master);
    FD_ZERO(&readfds);

    // Find a socket and bind to it
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((rv = getaddrinfo(NULL, argv[1], &hints, &ai)) != 0){
        fprintf(stderr, "server: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next){
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(listener < 0){
            continue;
        }
        
        // Lose the address already in use error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if(bind(listener, p->ai_addr, p->ai_addrlen) < 0){
            close(listener);
            continue;
        }

        break;

    }

    if(p == NULL){
        fprintf(stderr, "server: failed to bind\n");
        exit(2);
    }
    
    freeaddrinfo(ai);   // all done with this struct

    // listen for incoming connections
    if(listen(listener, BACKLOG) == -1){
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener;

    // Main loop
    for(;;){
        readfds = master; // Copy the file descriptor list
        if(select(fdmax+1, &readfds, NULL, NULL, NULL) == -1){
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data read
        for(i = 0; i <= fdmax; i++){
            if(FD_ISSET(i, &readfds)){
                if(i == listener){
                    // Handle new connections
                    addrlen = sizeof(remoteaddr);
                    newsocket = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

                    if(newsocket == -1){
                        perror("accpet");
                    }else{
                        FD_SET(newsocket, &master); // add new fd to master set
                        if(newsocket > fdmax){
                            fdmax = newsocket + 1;
                        }
                        printf("server: new connection from %s on socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newsocket);
                    }
                }else{
                    // handle data from a client
                    if((bytes = recv(i, buffer, sizeof(buffer), 0)) <= 0){
                        if(bytes == 0){
                            // connection closed
                            printf("server: socket %d disconnected\n", i);
                        }else{
                            perror("recv");
                        }
                        close(i);
                        FD_CLR(i, &master); // remove from master set
                    }else{
                        // we got some data from a client
                        for(j = 0; j < fdmax; j++){
                            if(FD_ISSET(j, &master)){
                                if(j != listener && j != i){
                                    if(send(j, buffer, bytes, 0) == -1){
                                        perror("send");
                                    }
                                }
                            }
                        } 
                    } 
                } // END handle data from client
            }
        } 
    } 
    return 0;
}

void *get_in_addr(struct sockaddr *sa){
    if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



