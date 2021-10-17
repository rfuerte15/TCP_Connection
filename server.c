/*
    Author: Rafael Fuerte-Luna
    Course: 3530 -- Computer Networks
    Description: This program acts as a server in demonstrating the 3-Way Handshake in a TCP connection
                 as well as the closing protocol in the TCP protocol.
*/
#include <stdio.h>      
#include <stdlib.h>      
#include <string.h>  
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netdb.h>         
#include <netinet/in.h>
#include <errno.h>      
#include <unistd.h>     
#include <arpa/inet.h> 
#include "TCP.h"
#define MESSAGE 500

int main(int argc, char **argv)
{
    FILE *output;                       // output file stream
    struct TCP svr_segment,          // server TCP segment
               cli_segment;          // client TCP segment
/* C-Strings for Messages */
    char cli_message[MESSAGE];          // client messages and replies

/* File Descriptors */
    int listening_fd,               // listening socket file descriptor
        serving_fd;                 // accepted client connection file descriptor

/* Address Structs */
    struct sockaddr_in servaddr;    // struct for storing server's local address info

/* Functional Variables */
    unsigned short int portNum;             // user-given port number for client communication
    int msg_size;                           // size of messages transmitted

/* Variables for Parsing */
    
    unsigned short int cli_flags,           // client flags
                       checksum;            // server checksum
    unsigned int cli_sequenceNum,           // client's sequence number
                 svr_sequenceNum,           // server's sequence number
                 cli_ACK,                   // client ACK
                 svr_ACK;                   // server ACK


    /* Get the Port Number */
    if(argc > 1)
    {
        portNum = atoi(argv[1]);
    }
    else
    {
        printf("Please provide a port number.\n");
        exit(1);
    }

    /* AF_INET - IPv4 IP , Type of socket, protocol */
    if( (listening_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )  // create socket for listening | SOCK_STREAM == TCP
    {
        perror("Failed creating listening socket.\n");
        exit(EXIT_FAILURE);
    }

    /* Set up the server info */
    bzero(&servaddr, sizeof(servaddr));           // clear out any preexisting data 
    servaddr.sin_family = AF_INET;                // set up to use with internet connection
    servaddr.sin_addr.s_addr = htons(INADDR_ANY); // set the Server IP address
                                                  // INADDR_ANY == special IP address used when we don't know the IP address of our machine 
                                                  // htons() == used to convert the unsigned short integer from host to network byte order 
    servaddr.sin_port = htons(portNum);           // set the port number --> Pick another port number to avoid conflict with other students --> 22315
 
    /* Bind the above address details to the listening socket */
    if( (bind(listening_fd,  (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0)  // bind(int socket_FileDescriptor, cost struct sockaddr *address, socklen_t addressLength)
    {
        perror("Failed binding listening socket.\n");
        exit(EXIT_FAILURE);
    }

    /* Start listening for incoming connections */
    if( (listen(listening_fd, 10)) < 0 )               // listen(int socket_FileDescriptor, int queue_size)
    {
        perror("Failed listening for clients.\n");
        exit(EXIT_FAILURE);
    }

    /* Accept Connections */
    if( (serving_fd = accept(listening_fd, (struct sockaddr*)NULL, NULL)) < 0 )  
    {
        perror("Failed to accept connection to client.\n");
        exit(EXIT_FAILURE);
    }

    /* Listen for client SYN segment */
    printf("Getting client connection request...\n");                   // MARKER -- Client connection request
    initialize(&cli_segment);    // initialize client segment to zero
    
    if( (recv(serving_fd, &cli_segment, sizeof(cli_segment), 0)) < 0 )
    {
        printf("Client SYN segment not received.\n");
        exit(1);
    }

    printf("Writing client segment to client.out file...\n");               // MARKER -- Writing to client.out file
    write_file(cli_segment, 'c');   // write client segment to client.out

    /* Configure ACK SYN to client's SYN segment */
    initialize(&svr_segment);                       // zero out server segment
    svr_segment.srcPort = portNum;                  // set server's port number
    svr_segment.destPort = cli_segment.srcPort;     // set destination (client) port number
    svr_segment.seqNum = 7;                         // initialize the sequence number
    svr_segment.ackNum = cli_segment.seqNum + 1;    // set the ACK number
    svr_segment.flags = 18;                         // set ACK SYN number --> 18 == 010010
    svr_segment.headerLen = sizeof(svr_segment);    // set header length
    checksumCalc(&svr_segment);                     // calculate and assign the checksum
    
    /* Send ACK SYN segment */
    printf("Sending SYN response to client...\n");                      // MARKER -- Sending SYN ACK to client
    if( (send(serving_fd, (struct TCP *) &svr_segment, sizeof(svr_segment), 0)) < 0)
    {
        printf("Error sending ACK SYN segment to client.\n");
        exit(1);
    }

    /* Listen for client ACK segment */
    initialize(&cli_segment);    // initialize client segment to zero
    
    printf("Listening for client ACK to SYN ACK...\n");                 // MARKER -- Client ACK to server SYN
    if( (recv(serving_fd, &cli_segment, sizeof(cli_segment), 0)) < 0 )
    {
        printf("Client ACK segment not received.\n");
        exit(1);
    }

    printf("Writing client response to client.out file...\n");              // MARKER -- Writing to client.out file
    write_file(cli_segment, 'c');   // write client segment to client.out

    /* Listen for client FIN segment */
    initialize(&cli_segment);    // initialize client segment to zero
    
    printf("Listening for client FIN segment...\n");                        // MARKER -- Listening for FIN segment
    if( (recv(serving_fd, &cli_segment, sizeof(cli_segment), 0)) < 0 )
    {
        printf("Client FIN segment not received.\n");
        exit(1);
    }

    printf("Writing client response to client.out file...\n");              // MARKER -- Writing to client.out file
    write_file(cli_segment, 'c');   // write client segment to client.out

    /* Configure ACK to client's FIN segment */
    svr_segment.ackNum = cli_segment.seqNum + 1;    // set ACK number
    svr_segment.flags = 16;                         // set ACK bit --> 16 == 010000
    svr_segment.checksum = 0;                       // temporarily zero out checksum
    svr_segment.headerLen = sizeof(svr_segment);    // set header length
    checksumCalc(&svr_segment);                     // calculate and set the checksum

    /* Send ACK to client's FIN segment */
    printf("Sending ACK to client's FIN segment...\n");                     // MARKER -- Sending ACK to client's FIN segment
    if( (send(serving_fd, (struct TCP *) &svr_segment, sizeof(svr_segment), 0)) < 0)
    {
        printf("Error sending ACK segment to client.\n");
        exit(1);
    }

    /* Configure FIN request segment */
    svr_segment.flags = 1;                          // set FIN bit --> 1 == 000001
    svr_segment.checksum = 0;                       // temporarily zero out checksum
    svr_segment.headerLen = sizeof(svr_segment);    // set header length
    checksumCalc(&svr_segment);                     // calculate and set the checksum

    /* Send FIN request segment */
    printf("Sending FIN segment to client...\n");                           // MARKER -- Sending FIN to client
    if( (send(serving_fd, (struct TCP *) &svr_segment, sizeof(svr_segment), 0)) < 0)
    {
        printf("Error sending FIN request segment to client.\n");
        exit(1);
    }

    /* Listen for client ACK */
    initialize(&cli_segment);    // zero out client segment
    
    printf("Listening for client final ACK...\n");                          // MARKER -- Listening for final client ACK
    if( (recv(serving_fd, &cli_segment, sizeof(cli_segment), 0)) < 0 )
    {
        printf("Client final ACK segment not received.\n");
        exit(1);
    }

    printf("Writing client response to client.out file...\n");           // MARKER -- Writing to client.out file
    write_file(cli_segment, 'c');   // write client segment to client.out

    printf("End of transmission\nConneciton closing...\n"); // mark end of the transmission
    close (serving_fd);                                     // close the current connection
    close(listening_fd);                                    // close listening socket when disconnecting server
}
