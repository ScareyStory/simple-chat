/*******************************************************************************
** Author:        Story Caplain
** Filename:      chatclient.c
** Description:   This file is the client in the chat.
**                It uses sockets to communicate via TCP
** Course name:   CS 372
** Last modified: 02/09/2020
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

/*******************************************************************************
** Sources:
** https://beej.us/guide/bgnet/pdf/bgnet_usl_c_1.pdf
** Socket lectures from CS 344 at Oregon State University
*******************************************************************************/

/* buffer lengths */
const size_t max_buffer = 550;
const size_t name_buffer = 11;

/* Error function used for reporting issues */
void error(const char *msg) {fprintf(stderr,"%s\n",msg); exit(1);}

/* chatting function prototypes */
int sending(int, char*);
int receive(int);

int main(int argc, char** argv) {

    /* Much of the code below is pulled directly from my cs344 projects */
    /* variables to be used below */
    int socketFD, portNumber, charsWritten, charsRead, temp;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;

    /* first check that number of args is correct */
    if(argc != 3) {
        printf("\nchatclient must be called with host name"); 
        printf(" and port number as arguments!\n");
        exit(1);
    }

    /* convert port number to an int */
    portNumber = atoi(argv[2]);

    /* initialize char array for username */
    char handle[max_buffer];
    memset(handle,'\0',sizeof(handle));
    int i;

    /* get handle from user */
    printf("\nPlease enter your handle: ");
    fgets(handle, max_buffer, stdin);

    /* remove newline */
    for(i=0;i<max_buffer;i++) {
        if(handle[i] == '\n') {
            handle[i] = '\0';
        }
    }

    /* check that handle is not too long */
    while(handle[name_buffer-1] != '\0') {
        memset(handle,'\0',sizeof(handle));
        printf("\nHandle too long! Must be 10 characters or less");
        printf("\nPlease enter your handle: ");
        fgets(handle, max_buffer, stdin);

        /* remove newline */
        for(i=0;i<name_buffer;i++) {
            if(handle[i] == '\n') {
                handle[i] = '\0';
            }
        }
    }

    /* Set up the server address struct */
    /* Clear out the address struct */
    memset((char*)&serverAddress, '\0', sizeof(serverAddress));

    /* Create a network-capable socket */
    serverAddress.sin_family = AF_INET;

    /* Store the port number */
    serverAddress.sin_port = htons(portNumber);

    /* Set the hostname from the argument */
    serverHostInfo = gethostbyname(argv[1]);
    if(serverHostInfo == NULL) {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(1);
    }

    /* Copy in the address */
    memcpy((char*)&serverAddress.sin_addr.s_addr,
           (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

    /* Set up the socket */
    socketFD = socket(AF_INET, SOCK_STREAM, 0); /* Create the socket */
    if(socketFD < 0) error("CLIENT: ERROR opening socket");

    /* Connect to server */
    if(connect(socketFD,
        (struct sockaddr*)&serverAddress,sizeof(serverAddress))<0)
        /* Connect socket to address */
        error("CLIENT: ERROR connecting");

    /* inform of successful connection */
    printf("Successfully connected to server!\n");

    /* chat loop */
    while(1) {

        int check_send = 0;
        int check_recv = 0;
        check_send = sending(socketFD, handle);
        check_recv = receive(socketFD);

        /* if either check is 1 close the socket and break the loop */
        if(check_send == 1 || check_recv == 1) {
            close(socketFD);
            break;
        }
    }
    return 0;
}

/*******************************************************************************
** Function name: sending
## preconditions: Takes as parameters a socket and username created in main
## postconditions: After sending a message, the function returns a 1 or 0
##                 1 means that the user quit the chat
##                 0 means that a valid message was sent
******************************************************************************/ 
int sending(int socketFD, char* handle) {

    /* initialize char array for message */
    char message[max_buffer];
    memset(message,'\0',sizeof(message));

    /* initialize char array for chat */
    char chat[max_buffer];
    memset(chat,'\0',sizeof(chat));

    int temp = 0;
    int i = 0;

    /* get message from user */
    printf("\n%s: ", handle);
    fgets(message, max_buffer, stdin);

    /* remove newline */
    for(i=0;i<max_buffer;i++) {
        if(message[i] == '\n') {
            message[i] = '\0';
        }
    }

    /* check that message is not too long or empty*/
    while(message[500]!='\0' && message[0]!='\n' && message[0]!='\0') {
        memset(message,'\0',sizeof(message));
        printf("\nMessage must be 1-500 characters");
        printf("\n%s: ", handle);
        fgets(message, max_buffer, stdin);

        /* remove newline */
        for(i=0;i<max_buffer;i++) {
            if(message[i] == '\n') {
                message[i] = '\0';
            }
        }
    }

    /* check if the message is "/quit" */
    if(strstr(message,"/quit")) {
        printf("\nChat has been quit, goodbye...");
        temp = send(socketFD,message,sizeof(message),0);
        if(temp == -1) {
            error("chatclient failed to send /quit");
        }
        return 1;
    }

    /* combine parts of message */
    temp = sprintf(chat,"%s: %s",handle,message);
    if(temp == -1) {
        printf("chatclient failed to append message to handle");
        return 1;
    }

    /* send message */
    temp = 0;
    int charsWritten = 0;
    while(charsWritten < sizeof(chat)) {
        temp = send(socketFD,chat,sizeof(chat),0);
        /* if temp ever equals -1 an error occurred */
        if(temp==-1) {
            printf("chatclient failed to send message");
            return 1;
        }
        charsWritten += temp;
    }

    return 0;
}

/*******************************************************************************
** Function name:  receive
## preconditions:  Takes as parameters a socket 
## postconditions: After receiving a message, the function returns a 1 or 0
##                 1 means that the user quit the chat or chat failed
##                 0 means that a valid message was received
******************************************************************************/ 
int receive(int socketFD) {

    /* initialize char array for message */
    char message[max_buffer];
    memset(message,'\0',sizeof(message));

    int temp = 0;

    /* receive message */
    temp = recv(socketFD, message, sizeof(message), 0);
    /* see if recv failed */
    if(temp == -1) {
        printf("\nrecv call failed in chatclient.c\n");
        return 1;
    }

    /* see if message is /quit */
    if(strstr(message,"/quit")) {
        printf("\nServer has quit the chat\n");
        return 1;
    }

    /* else print the message */
    else {
        printf("\n%s",message);
        printf("\n");
    }

    return 0;
}

