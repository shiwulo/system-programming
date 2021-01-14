#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <sched.h>
#include <assert.h>

#define size 1200
long long table[size][size];
long long col_m;
long long row_m;
long long pseudo_rand=0;

//將timespec轉成nanoseconds
double ts_to_double(struct timespec t) {
	return (double)(t.tv_sec * 1000000000 + t.tv_nsec)/1000000000;
}

void initTable()
{
	int i, j;
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++) {
			pseudo_rand += 17749;
			table[i][j] = pseudo_rand;
		}
}

void sumCol()
{
	int i, j;
	for (j = 0; j < size; j++)
		for (i = 0; i < size; i++)
			col_m += table[i][j];
}

void sumRow()
{
	int i, j;
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			row_m += table[i][j];
}

int main()
{
	struct timespec row1, row2;
	struct timespec col1, col2;
	struct timespec init1, init2;
	struct timespec init3, init4;

	printf("size of table %f MB\n", (double)sizeof(table)/1024/1024);

	clock_gettime(CLOCK_MONOTONIC, &init1);
	initTable();
	clock_gettime(CLOCK_MONOTONIC, &init2);

	clock_gettime(CLOCK_MONOTONIC, &init3);
	initTable();
	clock_gettime(CLOCK_MONOTONIC, &init4);	

	clock_gettime(CLOCK_MONOTONIC, &row1);
	sumRow();
	clock_gettime(CLOCK_MONOTONIC, &row2);

	clock_gettime(CLOCK_MONOTONIC, &col1);
	sumCol();
	clock_gettime(CLOCK_MONOTONIC, &col2);
	
	//printResult();
	
	printf("col_m %lld, row_m %lld\n", col_m, row_m);
	printf("比較第一次初始化，和第二次初始化的時間差異\n");
	printf("init: %f\n", ts_to_double(init2)-ts_to_double(init1));
	printf("init: %f\n", ts_to_double(init3)-ts_to_double(init4));
	printf("比較row major和column major的速度的差異\n");
	printf("row major: %f\n", ts_to_double(row2)-ts_to_double(row1));
	printf("column major: %f\n", ts_to_double(col2)-ts_to_double(col1));

	return 0;
}
