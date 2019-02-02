#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t mutex;
sem_t semaphore1, semaphore2;
int gotoSleep;

int nsecpersec = 1000000000;
char* showTime() {
    static char timeStr[200];
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long nsec = ts.tv_sec * nsecpersec + ts.tv_nsec;
    sprintf(timeStr, "%ld.%ld", nsec/nsecpersec, nsec%nsecpersec);
    return timeStr;
}

void p(void* para) {
    for (int i=0; i< 100; i++) {
        pthread_mutex_lock(&mutex);
        //printf("%s, thread1 enter CS\n", showTime());
        sem_post(&semaphore1);
        if(gotoSleep) usleep(1);
        pthread_mutex_unlock(&mutex);
        sem_wait(&semaphore2);
    }
}

void q(void* para) {
    for(int i=0; i< 100; i++) {
        //printf("%s, thread2 try lock\n", showTime());
        sem_wait(&semaphore1);
        //printf("%s, thread2 enter CS\n", showTime());
        pthread_mutex_lock(&mutex);
        
        pthread_mutex_unlock(&mutex);
        sem_post(&semaphore2);
    }
}

int main(int argc, char** argv) {
	pthread_t id1, id2;
	pthread_mutexattr_t attr;

    if (argc >= 2)
        gotoSleep = 1;
    else gotoSleep = 0;
    
    sem_init(&semaphore1, 0, 0);
    sem_init(&semaphore2, 0, 0);
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ADAPTIVE_NP);
	pthread_mutex_init(&mutex, &attr);
	pthread_create(&id1,NULL,(void *) p,NULL);
	pthread_create(&id2,NULL,(void *) q,NULL);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	return (0);
}