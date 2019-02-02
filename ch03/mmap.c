#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const size_t k=1024;
const size_t m=k*k;
const size_t g=k*m;
const size_t t=k*g;
int main (int argc, char** argv) {
    int logfile_fd = open("./trace.log", O_CREAT|O_LARGEFILE|O_TRUNC|O_NOATIME, S_IRUSR|S_IWUSR);
    char* logString = mmap(NULL, 5*t, PROT_WRITE, MAP_PRIVATE | MAP_HUGE_1GB, logfile, logfile_fd, 0);
    for (int i=0; i< 1000000; i++) {
        logString += sprintf(logString, "%d\n", i);
    }
}