ifdef NODEBUG
WARNINGS = -w
else
WARNINGS = -Wall -Wno-deprecated-declarations
endif

ifdef NODEBUG
DEBUG = 
else
DEBUG = -g
endif

CC = gcc
CFLAGS = -c $(DEBUG) $(WARNINGS) 


all: test

test: malloc.o test.o
	$(CC) -o test malloc.o test.o

malloc.o: malloc.c
	$(CC) $(CFLAGS) malloc.c

test.o: test.c
	$(CC) $(CFLAGS) test.c
clear:
	rm -rf *.o test
