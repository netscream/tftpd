CC = gcc
CPPFLAGS =
CFLAGS = -std=c11 -D_XOPEN_SOURCE=700 -O2 -Wall -Wextra -Wformat=2 -lpthread
LDFLAGS =
LOADLIBES =
LDLIBS = 
INSTALLDIR = /usr/local/bin

.DEFAULT: all
.PHONY: all
all: tftpdserver

tftpdserver: debugging.o printing.o server.o tftpd.o
	${CC} ${CFLAGS} debugging.o printing.o server.o tftpd.o -o tftpd

debugging.o: libs/debugging.c
	${CC} ${CFLAGS} libs/debugging.c -c

printing.o: libs/printing.c
	${CC} ${CFLAGS} libs/printing.c -c

server.o: libs/server.c
	${CC} ${CFLAGS} libs/server.c -c

tftpd.o: tftpd.c
	${CC} ${CFLAGS} tftpd.c -c

install: tftpd
	install -m 0755 tftpd ${INSTALLDIR}

clean:
	rm -f *.o

distclean: clean
	rm -f tftpd
