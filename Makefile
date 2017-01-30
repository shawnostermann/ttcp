#
#	$Id: Makefile,v 1.6 2004/02/11 02:25:19 sdo Exp $
#

CC=gcc

PROG=	ttcp
MAN1=	ttcp.0
BINDIR=	/usr/local/bin
MANDIR=	/usr/local/man/cat

CFLAGS=-g -O2 -Wall -Werror -Wno-format-security -Wno-unused-const-variable

LDFLAGS=-g -lnsl -lsocket
LDFLAGS=-g


ttcp: ttcp.o ticks.o timeval.o

clean:
	/bin/rm -f *.o core ttcp
