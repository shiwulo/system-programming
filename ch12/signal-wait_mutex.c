#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>

pthread_mutex_t mutex;

void thread1(void) {
    int ret = pthread_mutex_unlock(&mutex);//喚醒thead2//post
	if (ret != 0) {
        errno=ret;  //蠻特別的，要自己設定errno
        perror("ulock");
    }
}

void thread2(void) {
    int ret = pthread_mutex_lock(&mutex);
    ret = pthread_mutex_lock(&mutex);
	if (ret != 0) {    //thread1等待thread2//wait
        errno=ret;  //蠻特別的，要自己設定errno
        perror("lock");
    }
}

int main(void) {
	pthread_t id1, id2;
    pthread_mutexattr_t attr; 
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutex_init(&mutex, &attr);	//如果傳的第二個參數是NULL，程式會卡死
	pthread_create(&id1,NULL,(void *) thread1,NULL); 
	pthread_create(&id2,NULL,(void *) thread2,NULL);
	pthread_join(id1,NULL); 
	pthread_join(id2,NULL);
}