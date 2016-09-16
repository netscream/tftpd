#include <stdio.h>
#include "libs/server.h"    // include for tftpd server

/*
 * Function main()
 * Handles the start of the server and all checks before running server
 */

int main(int argc, char *argv[])
{
	//check if enough arguments else report to user how to use program
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s [PORT] [DATA DIR]\n", argv[0]);
        return -1;
    }
    
    int PortNo; //define portnumber
    PortNo = atoi(argv[1]);
    char* DirecoryString = argv[2];

    /* Check if directory exists , quit if directory does not exits */
    DIR* CheckDir = opendir(DirecoryString); //assign pointer to the directory
    if (CheckDir == NULL)
    {
        perror(DirecoryString);
        return -1;
    }
    closedir(CheckDir); //close directory and delete memory alloc

	return runServer(PortNo, DirecoryString);
}