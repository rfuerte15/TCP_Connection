/*
    Author: Rafael Fuerte-Luna
    Course: 3530 -- Computer Networks
    Description: This file defines the functions specific to the TCP segment structure 
                 declared in the "TCP.h" file. This structure and functions were created 
                 for use in conjunction with a server and client for demonstrating the 
                 3-Way Handshake and closing protocols used in the TCP protocol.
*/
#include "TCP.h"

/* Constructor */
void initialize( struct TCP *obj)               // STATUS: Tested -> Good
{
    obj->srcPort = 0;       // sorce port #
    obj->destPort = 0;      // destination port #
    obj->seqNum = 0;        // sequence n#
    obj->ackNum = 0;        // ACK #
    obj->headerLen = 0;     // header length
    obj->reserved = 0;      // reserved section
    obj->flags = 0;         // flags: URG, ACK, PSH, RST, SYN, FIN bits
    obj->recv_window = 0;   // receive window
    obj->checksum = 0;      // checksum
    obj->urgData = 0;       // urgent data
    obj->options = 0;       // options
    obj->data = 0;          // data
}

/* Checksum calculator */
void checksumCalc(struct TCP *obj)       // STATUS: Tested -> Good
{
    unsigned short int checksum_array[12];
    unsigned int i, 
                 sum= 0, 
                 checksum;

    memcpy(checksum_array, &obj, 24);   // copy 24 bytes of data from the TCP segment

    /* Compute the sum */
    for(i=0; i<12; i++)
    {
        sum = sum + checksum_array[i];
    }

    /* First fold */ 
    checksum = sum >> 16;
    sum = sum & 0x0000FFFF;
    sum = checksum + sum;

    /* Second fold */
    checksum = sum >> 16;
    sum = sum & 0x0000FFFF;
    obj->checksum = checksum + sum;
}

/* File output */
void write_file(struct TCP obj, char host)      // STATUS: Tested -> Good
{
    FILE *output;
    char segment[500],
         temp[50];

    /* Open the file */
    // server
    if(host == 's')
    {
        if( (output = fopen("server.out", "a")) == NULL )
        {
            printf("Error opening server.out\n");
            exit(1);
        }
    }
    // client
    else if(host == 'c')
    {
        if( (output = fopen("client.out", "a")) == NULL )
        {
            printf("Error opening client.out\n");
            exit(1);
        }
    }
    // error handling
    else
    {
        printf("Error with selected host for file writting.\n");
        exit(1);
    }

    /* Clear the c-strings out */
    bzero(segment, 500);
    bzero(temp, 50);

    /* Construct Segement into c-string */
    sprintf(temp, "Source Port: %i\n", obj.srcPort);
    strcat(segment, temp);                  
    sprintf(temp, "Destination Port: %i\n", obj.destPort);      // destination port
    strcat(segment, temp);
    sprintf(temp, "Sequence Number: %i\n", obj.seqNum);         // sequence number
    strcat(segment, temp);
    sprintf(temp, "ACK: %i\n", obj.ackNum);                     // ACK
    strcat(segment, temp);
    sprintf(temp, "Header Length: %i\n", obj.headerLen);        // header length
    strcat(segment, temp);
    sprintf(temp, "Reserved: %i\n", obj.reserved);              // reserved
    strcat(segment, temp);
    sprintf(temp, "Flags: %i\n", obj.flags);                    // flags
    strcat(segment, temp);
    sprintf(temp, "Receive Window: %i\n", obj.recv_window);     // receive window
    strcat(segment, temp);
    sprintf(temp, "Checksum: %i\n", obj.checksum);              // checksum
    strcat(segment, temp);
    sprintf(temp, "Urgent Data: %i\n", obj.urgData);            // urgent data
    strcat(segment, temp);
    sprintf(temp, "Options: %i\n", obj.options);                // options
    strcat(segment, temp);
    sprintf(temp, "Data: %i\n\n", obj.data);                    // data
    strcat(segment, temp);
    
    /* Append string to file */
    fputs(segment, output);
    fclose(output);
}

/* Print segment contents */
void printSegment(struct TCP obj)               // STATUS: Tested -> Good
{
    printf("Source Port: %i\nDestination Port: %i\nSequence Number: %i\nACK: %i\nHeader Length: %i\nReserved: %i\nFlags: %i\nReceive Window: %i\nUrgent Data: %i\nOptions: %i\nData: %i\n\n", obj.srcPort, obj.destPort, obj.seqNum, obj.ackNum, obj.headerLen, obj.reserved, obj.flags, obj.recv_window, obj.urgData, obj.options, obj.data);
}