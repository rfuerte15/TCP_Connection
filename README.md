# TCP_Connection
Description: This program demonstrates the 3-Way Handshake used in a TCP connection 
             as well as the closing procedures involved in closing a TCP connection.
             The segment contents received on the server's end (client segments) are 
             written out to a client.out file while segment contents received on the
             client's end (server segments) are written out to a server.out file.

Compilation: 
    Server: gcc server.c TCP.c -o server
    Client: gcc client.c TCP.c -o client

Execution:
    Server: ./server [desired server port number]
    Client: ./client [port number used by server]
