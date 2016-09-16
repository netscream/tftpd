#ifndef SERVER_H
#define SERVER_H
#define buffSize 512
#define connectionLimit 5

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <arpa/inet.h> //for inet_ntop
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include "debugging.h"
#include "printing.h"

struct thread_zombie_kill {    /* Used as argument to thread_start() */
           pthread_t thread_id;        /* ID returned by pthread_create() */
           int       thread_num;       /* Application-defined thread # */
           pid_t     childPid;      /* From command-line argument */
       };
/* Define opcodes here with enumeration (0 = nothing)
 * 0 nothing
 * 1 read request
 * 2 wrtie request
 * 3 DATA
 * 4 acknowledgement
 * 5 ERROR // see error codes
 * */
enum opCode {NOOP, RRQ, WRQ, DATA, ACK, ERROR};
/* Define Error codes 
 * 0 nothing
 * 1 file not found
 * 2 access violation
 * 3 Disk full / alloc exceeded
 * 4 Uknown transfer id
 * 5 file allready exists
 * */
enum errorCode {NOERR, FNF, AV, DF, ITFTO, UTID, FAEX, NSUS};
/* 
 * Function list, to see description for each function please go to function implementation (in tftpd.c)
 *
 */
void sendAck(int sockfd, struct sockaddr_in clientAddr, socklen_t clientAddrLength, int blockNo);
void getFilenameFromMessage(char* message, int mSize, char* DataDir, char* fileToSend);
void errorMsg(char* message, int mSize, int errorNum, int sockfd, struct sockaddr_in clientAddr, socklen_t clientAddrLength);
void readRequest(char* message, int mSize, int sockfd, struct sockaddr_in clientAddr, socklen_t clientAddrLength, char* DataDir);
void decodeMessage(char* message, int mSize, int sockfd, struct sockaddr_in clientAddr, socklen_t clientAddrLength, char* DataDir);
int runServer(int PortNum, char* DataDir);
int checkPath(char* filename);
void *killZombieChildren(void* thePid);
#endif

