#include "server.h"

/*
 * Function runServer()
 * This function handles all (first) client requests and sends them to decodeMessage for further use.
 * returns to main for return on exit.
 */
int runServer(int PortNum, char* DataDir) 
{
    printBanner();
    printToOutput("Starting server"); //print to output with timestamp
    int sockfd = -1;
    int bSock = -1;
    struct sockaddr_in serverAddr, clientAddr;
    char message[buffSize];

    debugS("Create socket");
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Unable to create socket: ");
        return -1; // no use to continue if no socket, return -1 with error.
    }
    
    memset(&clientAddr, 0, sizeof(clientAddr)); //lets zero write clientaddr
    memset(&serverAddr, 0, sizeof(serverAddr)); //lets zero write serveraddr
    serverAddr.sin_family = AF_INET;                //set family = AF_INET
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);  //using any address on server
    serverAddr.sin_port = htons(PortNum);           // using PortNum as port
    
    /* Trying to bind server to port */
    debugS("Binding socket");
    bSock = bind(sockfd, (struct sockaddr *) &serverAddr, (socklen_t) sizeof(serverAddr));
    if (bSock == -1)
    {
        perror("Bind error:");
        return -1; // no use to continue if unable to bind socket
    }
    
    /* create client socket file descriptor set */
        fd_set readSocketFd;
        FD_ZERO(&readSocketFd);
        FD_SET(sockfd, &readSocketFd);
        
    /* Entering endless run of server loop */
    while(1) {
        int retSel = -1;
        pid_t newPID = 0;
        if ((retSel = select(sockfd+1, &readSocketFd, 0, 0, 0)) == -1)
        {
            perror("Select() error: ");
        }
        else if (retSel > 0)
        {
            debugS("Inside server loop");
            socklen_t clientAddrLength = sizeof(clientAddr);
            ssize_t nString = recvfrom(sockfd, message, sizeof(message), 0, (struct sockaddr*) &clientAddr, &clientAddrLength);
            if (nString == -1)
            { 
                perror("Receive error ");
                exit(1);
            }
            if ((newPID = fork()) == 0)
            {
                int newSock = socket(AF_INET, SOCK_DGRAM, 0);
                decodeMessage(message, nString, newSock, clientAddr, clientAddrLength, DataDir);
                exit(1);
            }
            else
            {   /* This is to be able to reap zombie children of parent, done with thread */
                struct thread_zombie_kill newZombie;
                newZombie.childPid = newPID;
                newZombie.thread_id = 0;
                pthread_attr_t zombieAttr;
                void* killResult;
                pthread_attr_init(&zombieAttr);
                pthread_create(&newZombie.thread_id, &zombieAttr, killZombieChildren, &newZombie);
                pthread_attr_destroy(&zombieAttr);
                pthread_join(newZombie.thread_id, &killResult);
                free(killResult);
            }
        }
    }

    return 0;
}

/* 
 * Function killZombieChildren
 * void pointer function to kill all zombie children after fork
 */
void *killZombieChildren(void* thePid)
{
    struct thread_zombie_kill *theZombie = thePid;
    pid_t childPid = theZombie->childPid;
    int status = 0;
    int waitStatus = 0;
    do 
    {
        waitStatus = waitpid(childPid, &status, WUNTRACED | WCONTINUED);
        if (waitStatus == -1)
        {
            perror("Child not reaped correctly: ");
            break;
        }
    }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    return 0;
}

/*
 * Function sendAck()
 * For sending ack messages to client
 */
void sendAck(int sockfd, struct sockaddr_in clientAddr, socklen_t clientAddrLength, int blockNo)
{
    debugS("Inside sendAck");
    size_t ackSize = 4;
    char ackArr[ackSize];
    ackArr[0] = 0;
    ackArr[1] = ACK;
    ackArr[2] = 0;
    ackArr[3] = blockNo;
    int ackSent = sendto(sockfd, &ackArr, ackSize, MSG_DONTWAIT, (struct sockaddr*) &clientAddr, clientAddrLength);
    
    if (ackSent == -1)
    {
        perror("Ack error:");
        exit(0);
    }
}

/*
 * Function getFilenameFromMessage
 * void function to return concat file to get with data directory assigned
 */
void getFilenameFromMessage(char* message, int mSize, char* DataDir, char* fileToSend)
{
    //char mesBack[mSize+strlen(DataDir)-1];
    strncpy(fileToSend, DataDir, strlen(DataDir));
    fileToSend[strlen(DataDir)] = '/';
    strncpy(fileToSend+strlen(DataDir)+1, message+2, mSize-2);
}

/* 
 * Function errorMsg()
 * Handles all error sending to client
 */
void errorMsg(char* message, int mSize, int errorNum, int sockfd, struct sockaddr_in clientAddr, socklen_t clientAddrLength)
{
    printToOutput("Sending errorMsg");
    size_t mesSize = mSize+5;
    int retErr = 0;
    char errorArr[mesSize];

    /* initalize errorMessage header */
    errorArr[0] = 0;
    errorArr[1] = ERROR;
    errorArr[2] = 0;
    errorArr[3] = errorNum;
    for (size_t i = 4; i < mesSize; i++)
    {
        errorArr[i] = message[i-4];
    }
    errorArr[mesSize-1] = '\0'; /* terminate with 00 */

    debugSockAddr("clientAddr = ", clientAddr);
    retErr = sendto(sockfd, &errorArr, mesSize, MSG_DONTWAIT, (struct sockaddr*) &clientAddr, clientAddrLength);
    if (retErr == -1)
    {
        perror("Sendto failed with ");
        exit(0);
    }
    printToOutputError(message, clientAddr);
}

/*
 * Function readRequest
 * To handle all RRQ requests from clients
 */
void readRequest(char* message, int mSize, int sockfd, struct sockaddr_in clientAddr, socklen_t clientAddrLength, char* DataDir)
{
    /* For debugging function */
    debugS("Inside readRequest");
    debugMessage(message, mSize);
    /* end of debugging */

    char fileName[strlen(DataDir)+mSize-2];
    getFilenameFromMessage(message, mSize, DataDir, fileName);
    /* debug String write filename to output */
    debugS(fileName); //for debugging of filename
    printToOutputRequest(1, fileName, clientAddr); 
    if (checkPath(fileName) == 0)
    {
        char* errMes = "Access denied";
        errorMsg(errMes, 13, AV, sockfd, clientAddr, clientAddrLength);
        return;
    }
    FILE* fileToSend = fopen(fileName, "r");
    if (fileToSend == NULL)
    {
        char* errMes = "File not found";
        debugS(errMes);
        errorMsg(errMes, 20, FNF, sockfd, clientAddr, clientAddrLength);
    }
    else
    {
        char i = 1;
        char j = 0;
        int fileByteCount = 0;
        char dataToSend[buffSize+4];
        memset(dataToSend, 0, buffSize+4); // initalize with zeros (prevente unconditional jumps in valgrind)
        while(1)
        {
            char dataToReceive[buffSize+4];
            int sendErr = 0;

            if (dataToSend[3] != i)
            {
                memset(&dataToReceive, 0, buffSize+4); // initalize with zeros
                fileByteCount = fread(dataToSend+4, 1, 512, fileToSend); // read from file up to 512 bytes per round.
                dataToSend[0] = 0x0;
                dataToSend[1] = DATA;
                dataToSend[2] = j; // if i exceeds (1111 1111 = 255 or -128 to 127)
                dataToSend[3] = i;
            }
            debugD("Sending data block of size = ", fileByteCount+4);
            debugD("Opcode for sending = ", dataToSend[1]);
            
            
            sendErr = sendto(sockfd, dataToSend, fileByteCount+4, 0, (struct sockaddr*) &clientAddr, clientAddrLength);
            if (sendErr == -1)
            {
                perror("Sending Error: ");
                char* errMes = "Something went wrong please try again";
                errorMsg(errMes, strlen(errMes), NOERR, sockfd, clientAddr, clientAddrLength);
            }

            if (fileByteCount == 0)  
            { //lets break this loop if last packet and also if there is an edge case (512 byte exact size of file)
                break; // This will be big overhead for small files, but very small overhead for larger files
            } 

            debugS("Wait for ACK from client");

            fd_set readSocketFd;
            FD_ZERO(&readSocketFd);
            FD_SET(sockfd, &readSocketFd);
            
            struct timeval tv;
            tv.tv_sec = 10;
            tv.tv_usec = 0;
            int retSel = -1;
        
            if ((retSel = select(sockfd+1, &readSocketFd, 0, 0, &tv)) == -1)
            {
                perror("Select() error: ");
                break;
            }
            else if (retSel > 0)
            {
                int recvErr = recvfrom(sockfd, dataToReceive, sizeof(dataToReceive), 0, (struct sockaddr*) &clientAddr, &clientAddrLength);
                if (recvErr == -1)
                {
                    perror("Receive error");
                    //sendAck(sockfd, clientAddr, clientAddrLength, i);
                    break;
                }
            }
            else
            {
                printToOutputRequest(2, fileName, clientAddr); 
                break;
            }

            if (dataToReceive[1] != ACK)
            {
                perror("Something wrong with received msg");
                break; //something happened lets return (will also execute if recverr == -1 , that in next client retry)
            }
            else
            {
                debugS("Ack received");
                debugMessage(dataToReceive, sizeof(dataToReceive));
                debugD("j = ", j);
                debugD("i = ", i);
                if(dataToReceive[3] == i)
                {
                    if (dataToReceive[3] == -1)
                    {
                        j++;
                        i = 0;
                    }
                    else
                    {
                        i++;
                    }
                }
            }
        }
        printToOutputRequest(3, fileName, clientAddr); 
        fclose(fileToSend);
    }
}
 
/*
 * Function writeRequest()
 * For handling all WRQ requests from client
 */
void writeRequest(char* message, int mSize, int sockfd, struct sockaddr_in clientAddr, socklen_t clientAddrLength, char* DataDir)
{
    debugS("Inside writeRequest");
    debugMessage(message, mSize);
    debugS(DataDir);

    /* Send error message for need implementation */
    char* errMes = "Not allowed";
    debugS(errMes);
    errorMsg(errMes, 20, AV, sockfd, clientAddr, clientAddrLength);

}

/*
 * Function decodeMessage()
 * For deciding what function to call from opcode of message received from client
 */
void decodeMessage(char* message, int mSize, int sockfd, struct sockaddr_in clientAddr, socklen_t clientAddrLength, char* DataDir)
{
    switch (message[1])
    {
        case RRQ:
            printToOutput("Got read request");
            readRequest(message, mSize, sockfd, clientAddr, clientAddrLength, DataDir);
            break;
        case WRQ:
            printToOutput("Got write request");
            writeRequest(message, mSize, sockfd, clientAddr, clientAddrLength, DataDir);
            break;
        case ACK:
            printToOutput("Got ack packet");
            sendAck(sockfd, clientAddr, clientAddrLength, 0);
            break;
        default:
            {
                char* errMes = "Opcode not in use";
                errorMsg(errMes, 14, NOERR, sockfd, clientAddr, clientAddrLength);
            }
            break;
    }  
}
/*
 * Function checkpath()
 * to check for folder change
 */
int checkPath(char* filename)
{
    if ((strstr(filename, "../")) != NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}