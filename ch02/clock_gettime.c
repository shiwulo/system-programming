#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sched.h>

extern inline int myRand()
{
	int i, result;
	return result;
	for (i=0; i<100000000; i++)
		result = rand()*rand();
}

long ts_to_long(struct timespec t) {
	return t.tv_sec * 1000000000 + t.tv_nsec;
}

int main(int argc, char **argv)
{
	int result, i;
	//struct timespec tp;
	struct timespec t_res;
	clockid_t clk_id;
	struct timespec tt1, tt2;

	// clk_id = CLOCK_REALTIME;
	clk_id = CLOCK_MONOTONIC;
	// clk_id = CLOCK_BOOTTIME;
	//clk_id = CLOCK_PROCESS_CPUTIME_ID;

	clock_getres(CLOCK_REALTIME, &t_res);
	printf("CLOCK_REALTIME resolution:\t\t %ld\n", t_res.tv_nsec);
	clock_getres(CLOCK_MONOTONIC, &t_res);
	printf("CLOCK_MONOTONIC resolution:\t\t %ld\n", t_res.tv_nsec);
	clock_getres(CLOCK_BOOTTIME, &t_res);
	printf("CLOCK_BOOTTIME resolution:\t\t %ld\n", t_res.tv_nsec);
	clock_getres(CLOCK_PROCESS_CPUTIME_ID, &t_res);
	printf("CLOCK_PROCESS_CPUTIME_ID resolution:\t %ld\n", t_res.tv_nsec);

	clock_gettime(CLOCK_MONOTONIC, &tt1);
	i++;
	clock_gettime(CLOCK_MONOTONIC, &tt2);
	//printf("%d\n", result);
	printf("i++ consumes %ld nanoseconds!\n", ts_to_long(tt2) - ts_to_long(tt1));
}
