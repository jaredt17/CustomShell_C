CC = gcc
#CFLAGS = -ggdb -Wall -pedantic
CFLAGS = -o -Wall -pedantic -lm

all: shell

shell: shell.c error.o header.h

clean:
	rm -f *.o shell