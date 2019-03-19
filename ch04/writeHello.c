#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int fd;
    off_t begin;
    ssize_t wrt_ret;
    fd = open (argv [1], O_WRONLY);
    printf("fd = %d is opened\n", fd);
    sscanf(argv[2], "%ld", &begin);
    lseek(fd, begin, SEEK_SET);
    wrt_ret=write(fd, "hello", sizeof("hello"));
	printf("wrt_ret = %ld", wrt_ret);
}