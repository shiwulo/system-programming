SHELL = /bin/bash
CC = gcc
CFLAGS = -g -O3 -pthread
SRC = $(wildcard *.c)
EXE = $(patsubst %.c, %, $(SRC))

all:	${EXE}

%:		%.c
	${CC} ${CFLAGS} $@.c -o $@

clean:
	rm ${EXE}