#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sched.h>
#include <assert.h>

/*
The RDTSCP instruction reads the same TSC as the RDTSC instruction, so if RDTSC is invariant, then RDTSCP will be as well.
RDTSCP is slightly more ordered than RDTSC.  RDTSC is not ordered at all, which means that it will execute some time in the out-of-order window of the processor, which may be before or after the instruction(s) that you are interested in timing.   RDTSCP will not execute until all prior instructions (in program order) have executed.  So it can't execute "early", but there is no guarantee that the execution won't be delayed until after some subsequent (in program order) instructions have executed.
*/
extern __inline__ uint64_t rdtscp(void)
{
    uint32_t lo, hi;
    // take time stamp counter, rdtscp does serialize by itself, and is much cheaper than using CPUID
    __asm__ __volatile__("rdtscp":"=a"(lo), "=d"(hi));
    return ((uint64_t) lo) | (((uint64_t) hi) << 32);
}

long ts_to_long(struct timespec t) {
	return t.tv_sec * 1000000000 + t.tv_nsec;
}

int main(int argc, char **argv)
{
    int tmp;
    uint64_t cycles1, cycles2;
    struct timespec ts1, ts2;

    printf("這個程式是量測一個指令執行的時間，但CPU可同時執行數十個指令\n");
    printf("因此這些量測方法比較適合量測大範圍的程式碼\n\n");

    cycles1 = rdtscp();
    tmp++;
    cycles2 = rdtscp();

    clock_gettime(CLOCK_MONOTONIC, &ts1);
    tmp++;
    clock_gettime(CLOCK_MONOTONIC, &ts2);

    printf("開始 %lu, 結束 %lu\n",cycles1, cycles2);
    printf("rdtscp: tmp++ consumes %lu cycles!\n", cycles2-cycles1);
    printf("開始 %lu, 結束 %lu\n",ts_to_long(ts1), ts_to_long(ts2));
    printf("clock_gettime: tmp++ consumes %lu nanoseconds!\n", ts_to_long(ts2)-ts_to_long(ts1));
    assert(system("cat /proc/cpuinfo | grep 'cpu MHz' | head -1")>=0);
}
