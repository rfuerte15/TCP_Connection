/*
    Author: Rafael Fuerte-Luna
    Course: 3530 -- Computer Networks
    Description: This file declares the TCP segment structure and its functions. This
                 structure and functions were created for use in conjunction with a server
                 and client for demonstrating the 3-Way Handshake and closing protocols used
                 in the TCP protocol.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* TCP Objects for Enabling 3-Way Handshake */
struct TCP{

/* Variables */
    unsigned short int srcPort,         // source port # (16-bit)
                       destPort;        // destination port # (16-bit)
    unsigned int seqNum;                // sequence number (32-bit)
    unsigned int ackNum;                // acknowledgement number (32-bit)
    unsigned int headerLen;             // header length
    unsigned int reserved : 4;          // reserved section (?) (4-bit)
    unsigned int flags : 6;             // flags (6-bit): URG, ACK, PSH, RST, SYN, FIN bits
    unsigned short int recv_window;     // receive window (16-bit)
    unsigned short int checksum;        // checksum (16-bit)
    unsigned short int urgData;         // urgent data pointer (16-bit)
    unsigned int options;               // options (32-bit)
    unsigned int data;                  // data                                                
};

/* Constructor */
void initialize(struct TCP *obj);               // STATUS: Tested -> Good

/* Checksum calculator */
void checksumCalc(struct TCP *obj);             // STATUS: Tested -> Good

/* File output */
void write_file(struct TCP obj, char host);     // STATUS: Tested -> Good

/* Print segment contents */
void printSegment(struct TCP obj);              // STATUS: Tested -> Good