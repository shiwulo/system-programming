all: pointer hello table rdtsc clock_gettime clock_gettime_loop pointer1

pointer: pointer.c
	gcc pointer.c -g -o pointer
hello: hello.c
	gcc hello.c -g -o hello
table: table.c
	gcc table.c -g -o table
rdtsc: rdtsc.c
	gcc rdtsc.c -g -o rdtsc
clock_gettime: clock_gettime.c
	gcc clock_gettime.c -g -o clock_gettime
clock_gettime_loop: clock_gettime_loop.c
	gcc clock_gettime_loop.c -g -o clock_gettime_loop
pointer1: pointer1.c
	gcc pointer1.c -g -o pointer1
clean:
	rm pointer hello table rdtsc clock_gettime clock_gettime_loop pointer1

