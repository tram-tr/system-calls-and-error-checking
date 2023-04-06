CC = gcc
CFLAGS = -Wall -g -std=c99

all: filecopy treecopy

filecopy: filecopy.c
	$(CC) $(CFLAGS) -o $@ $^

treecopy: treecopy.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f filecopy treecopy
