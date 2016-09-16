#include "printing.h" //include header file
/* 
 * Function printToOutput
 * for printing to output with a timestamp
 */
void printToOutput(char* message)
{
    printTime();
    fprintf(stdout, "[+] %s\n", message);
}

/* 
 * Function printToOutputRequest
 * for printing to output from requests with a timestamp
 */
void printToOutputRequest(int whatToWrite, char* fileName, struct sockaddr_in clientAddr)
{
	/* whattowrite equals to
	 *  1: Print out file request from client
	 *  2: Print out timeout error if server does not receive ack from client within 10 sec
	 *  3: Print out the finish of file transfer
	 */
	if (whatToWrite == 1)
    {
    	printTime();
    	fprintf(stdout, "[+] File \"%s\" request from %s:%d\n", fileName, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    }
    else
    if (whatToWrite == 2)
    {
    	printTime();
    	fprintf(stdout, "[+] File \"%s\" timout error for %s:%d\n", fileName, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    }
    else
    if (whatToWrite == 3)
    {
    	printTime();
    	fprintf(stdout, "[+] File \"%s\" SUCCESS sent to %s:%d\n", fileName, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));	
    }
}

/* 
 * Function printToOutput
 * for printing to output from errors with a timestamp
 */
void printToOutputError(char* message, struct sockaddr_in clientAddr)
{
    printTime();
    fprintf(stdout, "[+] Error message: \"%s\" sent to %s:%d\n", message, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
}

/*
 * Function printBanner
 * only for printing banner to stdout
 */
void printBanner()
{
    printf("------------------------------------------------\n");
    printf("|   TFTPD server for tsam course               |\n");
    printf("|   Authors:                                   |\n");
    printf("|   Hlynur Hansen, hlynur14@ru.is              |\n");
    printf("|   Magnus Vilhelm Bjornsson, magnusvb14@ru.is |\n");
    printf("|   Þórdís Magnúsdóttir, thordis14@ru.is       |\n");
    printf("------------------------------------------------\n");
}