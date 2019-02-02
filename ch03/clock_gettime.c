#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

long long nspersec = 1000000000;
long long timespec2nsec(struct timespec ts) {
    long long ns = ts.tv_nsec;
    ns += ts.tv_sec * nspersec;
    return ns;
    //return (double)ns/1000000000.0;
}

int main(int argc, char** argv) {
    struct timespec ts_begin;
    struct timespec ts_end;
    long long begin_ns;
    long long end_ns;
    long long ns=0;
    int i;
    for (int i=0; i<100000000; i++) {
        clock_gettime(CLOCK_REALTIME, &ts_begin);
        clock_gettime(CLOCK_REALTIME, &ts_end);
        ns += timespec2nsec(ts_end)-timespec2nsec(ts_begin);
    }
    printf ("%f ns\n", (double)ns/100000000);
}
