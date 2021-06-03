#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdatomic.h>

int global=0;
int global1=0, global2=0;
pthread_mutex_t mutex;

// 1. thread create join (fork, wait。)
// 2. 共用變數的修改要『想辦法』『確保一次只有一個人改』
// 3. 可以將一個共用變數，變成數個，然後再合併結果

void thread(void* para) {
	int i;
	int* tmp=(int*)para;
	for (i=0; i<1000000000; i++) {
		//pthread_mutex_lock(&mutex);     //進入critical section
		//global+=1;	//cache coherence
		//atomic_fetch_add(&global, 1);
		(*tmp)++;
		//pthread_mutex_unlock(&mutex);   //離開critical section
	}
}

int main(void) {
	pthread_t id1, id2;
    int global3=0, global4=0;
	//pthread_mutex_init(&mutex, NULL);	//mutex預設是unlock
	//pthread_create(&id1,NULL,(void *) thread,&global1); 
	//pthread_create(&id2,NULL,(void *) thread,&global2);
    pthread_create(&id1, NULL, (void *)thread, &global3);
    pthread_create(&id2, NULL, (void *)thread, &global4);

    pthread_join(id1,NULL); 
	pthread_join(id2,NULL);
	global = global1 + global2;
	printf("1000000000+1000000000 = %d\n", global);
}