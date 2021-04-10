SHELL = /bin/bash
CC = gcc
CFLAGS = -O3 -pg -g -march=skylake	-mavx512f -finstrument-functions -z lazy -lrt
SRC = $(wildcard *.c)
EXE = $(patsubst %.c, %, $(SRC))

all: ${EXE}

%:	%.c
	${CC} ${CFLAGS} $@.c -o $@

clean:
	rm ${EXE}
