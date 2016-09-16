#ifndef DEBUGGING_H
#define DEBUGGING_H
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h> //for inet_ntop
#include <time.h>

/* 
 * Change this value to 1 (or something other than 0) to get debug function to work for tftp server 
 *
 */
#define deBug 0

/* 
 * Function to print timestamp to output
 */
void printTime();
void debugS(char* message);
void debugD(char* message, int id);
void debugSockAddr(char* message, struct sockaddr_in clientAddr);
void debugMessage(char* message, size_t mSize);
#endif