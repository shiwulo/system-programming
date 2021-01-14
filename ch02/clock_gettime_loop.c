#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sched.h>

//將timespec轉成nanoseconds
long ts_to_long(struct timespec t) {
	return t.tv_sec * 1000000000 + t.tv_nsec;
}

int main(int argc, char **argv)
{
	clockid_t clk_id;
	struct timespec tt;
	//開始及結束時間
	struct timespec tt1, tt2;
	const int loopSize =1000;

	//clk_id = CLOCK_REALTIME;
	//clk_id = CLOCK_BOOTTIME;
	//clk_id = CLOCK_PROCESS_CPUTIME_ID;
	clk_id = CLOCK_MONOTONIC;

	clock_gettime(clk_id, &tt1);
	for (int i=0; i<loopSize; i++) {
		clock_gettime(clk_id, &tt);
	}
	clock_gettime(clk_id, &tt2);
	printf("約略估計clockgettime的精准度 %f 奈秒\n", (double)(ts_to_long(tt2)-ts_to_long(tt1))/loopSize);
}
