/*
Client software for my chat server.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXBUFFER 1024

int sendMsg(int sock, char *buff);
void clearBuffer(char *buff);
void *get_in_addr(struct sockaddr *sa);
void *recv_run(void *sock);

int main(int argc, char **argv){

    int clientSocket;
    int transmit = 1;
    char buffer[MAXBUFFER];
    struct addrinfo hints, *serverInfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    pthread_t recvThread;

    // Make sure we have the correct number of cmd line args
    if(argc != 3){
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(1);
    }

    // Set up addr info struct
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // Try and get an available address
    if((rv = getaddrinfo(argv[1], argv[2], &hints, &serverInfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = serverInfo; p != NULL; p = p->ai_next){
        
        // Create Socket
        if((clientSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("client: socket");
            continue;
        }

        // Attempt to connect our socket to specified address and port
        if(connect(clientSocket, p->ai_addr, p->ai_addrlen) == -1){
            close(clientSocket);
            perror("client: connect");
            continue;
        }

        break;
    }

    // If we were unable to find a valide address to connect to, throw an error.
    if(p == NULL){
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    // Convert our address into a printable format and print it
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
    printf("Client: connecting to %s\n", s);

    // Free up our addrinfo, we don't need it anymore
    freeaddrinfo(serverInfo);

    // Create our recieve thread
    pthread_create(&recvThread, NULL, recv_run, &clientSocket);

    // Main loop for sending messages
    while(transmit == 1){
        clearBuffer(buffer);                            // Clear out the buffer
        transmit = sendMsg(clientSocket, buffer);       // Try and send a message!
    }

    // Once we are disconnecting, close the socket and join the threads!
    pthread_join(recvThread, NULL);
    close(clientSocket); 

    return 0;
}

void *get_in_addr(struct sockaddr *sa){
    /* get sockaddr, IPv$ or IPv6 */
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int sendMsg(int sock, char *buff){
    /* Function to send data to a specific socket */
    fgets(buff, 1024, stdin);
    if(send(sock, buff, strlen(buff), 0) == -1){
        return 0;
    }else{
        printf("[ME] > %s\n", buff);
        return 1;
    }
}

void clearBuffer(char *buff){
    /* Function to set the buffer to 0 */
    int i;
    for(i = 0; i < MAXBUFFER; i++){
        buff[i] = 0;
    }
}


void *recv_run(void *sock){
    /* Function that takes in a socket as an argument for recieveing data */
    char *buffer = (char*)malloc(MAXBUFFER);
    while(1){
        clearBuffer(buffer);
        if(!recv(*(int*)sock, (char*)buffer, MAXBUFFER, 0)){
            printf("Server Error!\n");
            printf("Exiting!");
            close(*(int*)sock);
            free(buffer);
            exit(-1);
        }
        printf("[Anonymous] > %s\n", buffer);

    }
    free(buffer);
}

