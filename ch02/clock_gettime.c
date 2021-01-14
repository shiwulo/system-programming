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

//將timespec轉成nanoseconds
long ts_to_long(struct timespec t) {
	return t.tv_sec * 1000000000 + t.tv_nsec;
}

int main(int argc, char **argv)
{
	int result, i;
	//clock_gettime的精准度
	struct timespec t_res;
	//開始及結束時間
	struct timespec tt1, tt2;

	printf("不同參數下，clock_gettime的精準度\n");
	clock_getres(CLOCK_REALTIME, &t_res);
	printf("CLOCK_REALTIME resolution:\t\t %ld nanoseconds\n", t_res.tv_nsec);
	clock_getres(CLOCK_MONOTONIC, &t_res);
	printf("CLOCK_MONOTONIC resolution:\t\t %ld nanoseconds\n", t_res.tv_nsec);
	clock_getres(CLOCK_BOOTTIME, &t_res);
	printf("CLOCK_BOOTTIME resolution:\t\t %ld nanoseconds\n", t_res.tv_nsec);
	clock_getres(CLOCK_PROCESS_CPUTIME_ID, &t_res);
	printf("CLOCK_PROCESS_CPUTIME_ID resolution:\t %ld nanoseconds\n", t_res.tv_nsec);

	printf("印出執行『i++』花多少時間\n");
	clock_gettime(CLOCK_MONOTONIC, &tt1);
	i++;
	clock_gettime(CLOCK_MONOTONIC, &tt2);
	printf("i++ consumes %ld nanoseconds!\n", ts_to_long(tt2) - ts_to_long(tt1));
}
