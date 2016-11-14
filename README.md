# First socket programming project for computer networs

Readme for tftp server v0.3 *Still in beta*

  *Table of contents*

1 => About application
	|- info about this application
2 => Usage
	|- how to use this application
3 => Implement description
	|- Description about how the server is implemented
4 => File tree
	|- File tree and comments about each file
5 => Comments
	|- Other comments about application
6 => Version History
	|- Version history for implemented features


---------------------------------------------------------------
1 => About application

This is a Tftp server implementation as a assignment for TSAM course at Reykjavik University.

For this implementation we used the book Computer Networking - A Top-Down Approach, by James F Kurose and Keith W Ross,
also knowledge to implement different features and about each feature was optained from URL:http://linux.die.net/man/ .

---------------------------------------------------------------
2 => Usage

./tftpd [PORT] [DATA DIR]

[PORT] - port number that is not in use on this system
[DATA DIR] - the directory that the server is going to use

For developement:
	$ make                (Will compile the server)
	$ make & make install (Will compile the server and install) (*needs evelated priveledges* **root access in most cases**)
	$ make distclean	  (Will clean all compiled .o and application files)
---------------------------------------------------------------
3 => Implement description

tftpd.c: 
	Checks arguments handed to the server for correct input, if port is a legal portname and if the data directory exists.
	Calls runServer() which is located inside server.c
--------------------------
server.c: (server.h)
	runServer():
		Runs the server in infinit loop and waits for connection to socket.
		when a message is received from socket it returns it to decodeMessage().

	decodeMessage():
		Checks what opcode the message contains and sends it to corresponding function.

	readRequest():
		Handles the readRequest from the client and sends the correct data if located in correct folder.

	writeRequest():
		At the moment it only sends error message about it not being allowed, but the function is here for later implementation.

	sendAck():
		A function that sends ACK to client.

	getFilenameFromMessage():
		Function that extracts a filename from message for use in fopen();

	checkPath():
		Checks if the path inside the filename does not try to get out of the folder that is currently loaded into the program.

	errorMsg():
		Function that is implemented to only send error messages to the client.
--------------------------
printing.c: (printing.h)
	printToOutput():
		Prints input message to the stdout, put concats a timestamp to the output.

	printToOutputRequest():
		Prints to stdout the request , filename and ipaddress:port of client connecting, also gets timestamp concated to the output.

	printToOutputError():
		Prints to the stdout all error messages that are sent to client, that is opcode error.

	printBanner():
		Only prints the beutyful banner on start, *So pretty*
--------------------------
debugging.c: (debugging.h)  // note this is only for debugging the applictaion.
	// to be able to use these functions please change the "#define debug" macro inside debugging.h to 1 etc. (#define debug 1)

	printTime():
		Function that only prints out the exact server time to the output (note: SERVER TIME)

	debugS():
		For writing strings to the output from other functions, concats the timestamp and "[DEBUG]" to the output so the user knows what info is what info.

	debugD():
		For writing strings with integer values to the stdout, concats timestamp and "[DEBUG]" to the string.

	debugSockAddr():
		for sockaddr and sockaddr_in debugging

	debugMessage():
		for debugging packages sent over the socket.

---------------------------------------------------------------
4 =>File tree for pa1 folder
.
├── AUTHORS		*List of authors*
├── data		*Data directory*
├── pa1.pdf		*PDF file for project, about implementation*
├── README		*THIS FILE!!*
└── src			*src folder*
    ├── devDoc		*Documentation for developement*
    │   └── info.txt	*Some info for developement*
    ├── libs		*Library folder*
    │   ├── debugging.c	*Debugging c file*
    │   ├── debugging.h *Debugging header file*
    │   ├── printing.c	*Printing c file*
    │   ├── printing.h	*Printing header file*
    │   ├── server.c	*Server c file*
    │   └── server.h	*Server header file*
    ├── Makefile	*Makefile for the server application*
    └── tftpd.c		*The main code file*

---------------------------------------------------------------
5 => Comments


---------------------------------------------------------------
6 => Version History

01.09.2016 *First day of implementat
ion*
	|- Feature list created
		|- What do we want this server to do?
	|- RFC 1350 printed and underlined how this application should function

02.09.2016
	1* tftpd.c created and main function implemented
	|- Functions created from list of features
	|- Argument checker for main() implemented
	|- Folder existance checker in main() implemented
	|- call to runServer() implemented
	!!!! main() function fully implemented
	|- Started to implement runServer()
	|- macros implemented
	|- enumerations implemented
	|- DEBUG print functions created and implemented

03.09.2016
	|- decodeMessage() function created (not implemented)
		|- sendack() function created (not implemented)
		|- readRequest() function created (not implemented)
		|- writeRequest() function created (not implemented)
		|- errorMsg() function created (not implemented)
	|- runServer() *continued implementation*

04.09.2016
	|- decodeMessage() implemented
	|- sendack() function implemented
	|- errorMsg() function implemented

05.08.2016
	|- readRequest() implemented
	|- writeRequest() implemented
06.08.2016
	|- minor fixes
08.08.2016
	|- Created concurrency
	|- process is forked, the child gets new socket for sending data to client
	|- when sending is finished it returns and exits
	|- pthread waits for the child to die and reaps the process
	|- then frees all that is suppose to be free'd
12.08.2016
	|- Added timeout error if no receive within 10 sec from client while sending data
	|- FINISHED
