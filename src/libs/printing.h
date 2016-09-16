#ifndef PRINTING_H
#define PRINTING_H
#include <stdio.h>
#include <netinet/in.h> //for sockaddr_in
#include <arpa/inet.h> //for inet_ntop
#include "debugging.h"

void printToOutput(char* message);
void printToOutputRequest(int whatToWrite, char* fileName, struct sockaddr_in clientAddr);
void printToOutputError(char* message, struct sockaddr_in clientAddr);
void printBanner();
#endif