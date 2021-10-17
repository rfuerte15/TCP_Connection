/*
    Author: Rafael Fuerte-Luna
    Course: 3530 -- Computer Networks
    Description: This program acts as a client in demonstrating the 3-Way Handshake in a TCP connection
                 as well as the closing protocol in the TCP protocol.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>  
#include <unistd.h>
#include "TCP.h"
#define MESSAGE 500

int main(int argc,char **argv)
{
    struct TCP cli_segment,                 // TCP Segment
               svr_segment;
    int svr_portNum,                        // server port number
        cli_portNum,                        // client port number
        sock_fd,                            // socket file descriptor
        n,                                  // used in while-loop for reading the server responses
        close_bool = 1;                     // boolean to exit out of the transmission do-while loop
    char serv_response[MESSAGE],            // server response string
         cli_message[MESSAGE],              // message to the proxy server
         temp_str[MESSAGE];
    struct sockaddr_in servaddr,            // struct for storing client's local address info
                       sin;                 // struct used for finding client's port number
    socklen_t len = sizeof(sin);            // used with "sin" to generate the port number
    unsigned short int checksum;            // checksum

    /* Get port number execution */
    if(argc > 1)
    {
        svr_portNum = atoi(argv[1]);   // get destination port
    }
    else
    {
        printf("ERROR: Execution must be in the format: [cli_executable] [destination_portNumber]\n");
        exit(0);
    }


    /* AF_INET - IPv4 IP , Type of socket, protocol*/
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);   // create the socket
    
    /* Set up the client address struct */   
    bzero(&servaddr, sizeof(servaddr));         // clear out any prexisting data in the address struct
    servaddr.sin_family = AF_INET;              // set up for internet connection
    servaddr.sin_port = htons(svr_portNum);         // set up to connect at server's port number - temp: 22000
 
    /* Convert IPv4 and IPv6 addresses from text to binary form */
	inet_pton(AF_INET, "129.120.151.94", &(servaddr.sin_addr)); // inet_pton(int address_family, const char address_string, void *destination) 
                                                                // Takes the ip address, converts it to binary form, and stores it in servaddr.sin_addr
 
 /* Connection Request */

    /* Connect to the server */
    connect(sock_fd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    
    /* Get the client's port number */
    printf("Retrieving the client port number...\n");               // MARKER -- Getting client port number
    if(getsockname(sock_fd, (struct sockaddr *)&sin, &len) == -1)
    {
        perror("getsockname");
    }
    else
    {
        cli_portNum = ntohs(sin.sin_port);  // store the client port number
    }

    /* Configure SYN segment */
    initialize(&cli_segment);                       // intialize the segment to zero
    cli_segment.srcPort = ntohs(sin.sin_port);      // set the client source port number
    cli_segment.destPort = svr_portNum;             // set destination (server) port number
    cli_segment.seqNum = 1;                         // initialize client's sequence number
    cli_segment.flags = 2;                          // set SYN flag --> 2 == 000010
    cli_segment.headerLen = sizeof(cli_segment);    // set header length
    checksumCalc(&cli_segment);                     // calculate and set the checksum

    /* Send SYN segment */
    printf("Sending connection request to server...\n");            // MARKER -- Sending connection request
    if( (send(sock_fd, (struct TCP *) &cli_segment, sizeof(cli_segment), 0)) < 0)
    {
        printf("Error sending connection request segment to server.\n");
        exit(1);
    }

    /* Listen for server ACK segment */
    printf("Listening for server ACK to connection request...\n");  // MARKER -- Listening for client ACK
    if( (recv(sock_fd, &svr_segment, sizeof(svr_segment), 0)) < 0)
    {
        printf("Error receiving server SYN ACK to connection request.\n");
        exit(1);
    }

    printf("Writing server response to server.out file...\n");      // MARKER -- Writing to server.out file
    write_file(svr_segment, 's');                   // write server response to server.out

    /* Configure ACK segment to server's SYN ACK */
    cli_segment.seqNum++;                           // increment the client sequence number
    cli_segment.ackNum = svr_segment.seqNum + 1;    // set ACK number
    cli_segment.flags = 16;                         // set ACK bit --> 16 == 010000
    cli_segment.checksum = 0;                       // temporarily set checksum to zero
    cli_segment.headerLen = sizeof(cli_segment);    // recalculate the header length
    checksumCalc(&cli_segment);                      // recalculate the checksum

    /* Send ACK segment to server */
    printf("Sending ACK to server's SYN ACK...\n");                 // MARKER -- Sending ACK to server's ACK SYN
    if( (send(sock_fd, (struct TCP *) &cli_segment, sizeof(cli_segment), 0)) < 0)
    {
        printf("Error sending ACK segment to server's SYN.\n");
        exit(1);
    }

    /* Configure FIN segment */
    cli_segment.seqNum = 1;                         // reset client sequence number
    cli_segment.ackNum = 0;                         // reset ACK number
    cli_segment.flags = 1;                          // set FIN bit --> 1 == 000001
    cli_segment.checksum = 0;                       // temporarily set checksum to zero
    cli_segment.headerLen = sizeof(cli_segment);    // recalculate the header length
    checksumCalc(&cli_segment);                      // recalculate the checksum

    /* Send FIN segment */
    printf("Sending FIN request to server...\n");                   // MARKER -- Sending FIN segment to server
    if( (send(sock_fd, (struct TCP *) &cli_segment, sizeof(cli_segment), 0)) < 0)
    {
        printf("Error sending FIN segment.\n");
        exit(1);
    }

    /* Listen for server's ACK segment */
    initialize(&svr_segment);       // reset server segment struct to zero
    
    printf("Listening for server's ACK to FIN request...\n");       // MARKER -- Listening for server's ACK to FIN request
    if( (recv(sock_fd, &svr_segment, sizeof(svr_segment), 0)) < 0 ) 
    {
        printf("Server ACK to FIN request not received.\n");
        exit(1);
    }
    
    printf("Writing server response to server.out file...\n");      // MARKER -- Writing to server.out file
    write_file(svr_segment, 's');   // write server response to server.out

    /* Listen for server's FIN segment */
    initialize(&svr_segment);       // reset server segment struct to zero
    
    printf("Listening for server's FIN segment...\n");              // MARKER -- Listening for server's FIN segment
    if( (recv(sock_fd, &svr_segment, sizeof(svr_segment), 0)) < 0 ) 
    {
        printf("Server's FIN segment not received.\n");
        exit(1);
    }
    
    printf("Writing server response to server.out file...\n");      // MARKER -- Writing to server.out file
    write_file(svr_segment, 's');   // write server response to server.out

    /* Configure ACK segment to server's FIN segment */
    cli_segment.seqNum++;                           // increment client sequence number
    cli_segment.ackNum = svr_segment.seqNum + 1;    // Set ACK number
    cli_segment.flags = 16;                         // set ACK bit --> 16 == 010000
    cli_segment.checksum = 0;                       // temporarily set checksum to zero
    cli_segment.headerLen = sizeof(cli_segment);    // recalculate the header length
    checksumCalc(&cli_segment);                      // recalculate the checksum

    /* Send ACK segment to server's FIN segment */
    printf("Sending ACK to server's FIN segment...\n");             // MARKER -- Sending ACK to server's FIN segment
    if( (send(sock_fd, (struct TCP *) &cli_segment, sizeof(cli_segment), 0)) < 0)
    {
        printf("Error sending ACK segment to server's FIN segment.\n");
        exit(1);
    }
    
    printf("End of transmission.\nConnection closing...\n");    // mark end of transmission
    close(sock_fd);                                             // close connection
}