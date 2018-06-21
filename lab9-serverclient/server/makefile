# Variables Decleration
CC = gcc
CFLAGS = -g -Wall -c

all: target

target: server.o common.o
	$(CC) server.o common.o -o server
	rm -f *.o

server.o: server.c common.h
	$(CC) $(CFLAGS) server.c -o server.o

common.o: common.c
	$(CC) $(CFLAGS) common.c -o common.o

clean:
	rm -f *.o server
