/*
使用方法:
pi_drand48_r 點的數量 thread的數量
*/
#define _GNU_SOURCE 
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include <string.h>
#include <assert.h>
#include <stdatomic.h>
#include <sched.h>

int num_thread = 4;
long long total_loopcount = 1000000000;
double global_hit=0.0;
pthread_mutex_t mutex;

atomic_int assigned_cpuid=0;

void thread(void* rand_buffer)
{
	double point_x,point_y;
	long hit=0;
	long i;
	int j;
	long local_loopcount = total_loopcount/num_thread;
	double rand_d;

	cpu_set_t cpu_mask;
    CPU_ZERO(&cpu_mask);
    CPU_SET((int)assigned_cpuid, &cpu_mask);
	atomic_fetch_add(&assigned_cpuid, 2);
    int ret = sched_setaffinity(0, sizeof(cpu_set_t), &cpu_mask);
    assert(ret==0);

	printf("local_loopcount =%ld\n", local_loopcount);
	for(i=0;i<local_loopcount;i++) {	
		/*使用drand48_r由於產生random number相關的資料結構都
		保存在rand_buffer，因此不需要lock-unlock機制*/
		drand48_r(rand_buffer, &rand_d);
		point_x = rand_d;
		drand48_r(rand_buffer, &rand_d);
		point_y = rand_d;
		if( (point_x*point_x + point_y*point_y) < 1.0)
			hit+=1;
	}
	printf("hit = %ld\n", hit);
	pthread_mutex_lock(&mutex);
	global_hit += hit;
	pthread_mutex_unlock(&mutex);
}


void main(int argc,char*argv[]) {
	pthread_t id[16];
	/*初始化drand48_r所要使用的資料結構*/
	struct drand48_data rand_buffer[16];
	int i;
	double pi = 0.0;
	double rand_d;

	printf("usage ./pi_drand48r [loopcount = 1000000000] [number_thread = 4]\n")

	if (argc >= 2)
		total_loopcount=atol(argv[1]);
	if (argc >=3)
		num_thread=atoi(argv[2]);
	pthread_mutex_init(&mutex,NULL);	
	for(i=0;i<num_thread;i++) {	
		/*給drand48_r相對應的random seed*/
		/*請注意這裡假設rand和drand48_r使用不同的隨機產生方法*/
		srand48_r(rand(), &rand_buffer[i]);
		drand48_r(&rand_buffer[i], &rand_d);	//驗證random number
		//printf("@buffer = %p\n", &rand_buffer[i]);	//印出buf的位址
		//printf("thread%d's seed = %f\n", i, rand_d);	//驗證random number
		/*將random buffer透過參數傳遞給thread*/
		pthread_create(&id[i],NULL,(void *)thread, &rand_buffer[i]);
	}
	for(i=0;i<num_thread;i++)
		pthread_join(id[i],NULL);

	pi = (double)4*(global_hit/total_loopcount);
	printf("pi = %.8lf\n",pi);
}
