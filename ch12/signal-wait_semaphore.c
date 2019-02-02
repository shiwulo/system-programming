#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>


sem_t semaphores;

void thread1(void) {
    printf("thread1: sleep 10 sec.\n");
    for (int i=0; i<10; i++){
        fprintf(stderr, "%d ", i);
        sleep(1);
    }
    printf("10\n");
    printf("thread1: post a signal.\n");
    int ret = sem_post(&semaphores);//喚醒thead2//post
}

void thread2(void) {
    printf("thread2: wait for thread1...\n");
    int ret = sem_wait(&semaphores);
    printf("thread2: continue\n");
}

int main(void) {
	pthread_t id1, id2;
    sem_init(&semaphores, 0, 0);
	pthread_create(&id1,NULL,(void *) thread1,NULL); 
	pthread_create(&id2,NULL,(void *) thread2,NULL);
	pthread_join(id1,NULL); 
	pthread_join(id2,NULL);
}