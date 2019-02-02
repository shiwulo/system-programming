#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

static unsigned long long rdtsc(void)
{
	unsigned int low, high;

	asm volatile("rdtsc" : "=a" (low), "=d" (high));

	return low | ((unsigned long long)high) << 32;
}

const long long nspersec = 1000000000;
inline long long timespec2nsec(struct timespec ts) {
    long long ns = ts.tv_nsec;
    ns += ts.tv_sec * nspersec;
    return ns;
    //return (double)ns/1000000000.0;
}

int main(int argc, char** argv) {
    //struct timespec ts_begin;
    //struct timespec ts_end;
    unsigned long long begin_ns=0;
    unsigned long long end_ns=0;
    unsigned long long ns=0;
    for (volatile int i=0; i<10000000; i++) {
        begin_ns=rdtsc();
        end_ns=rdtsc();
        ns = end_ns-begin_ns;
        printf("begin=%lld ns, end=%lld ns, ns =%lld\n",begin_ns, end_ns, ns);
    }
    printf ("%f ns\n", (double)ns/10000000);
}
