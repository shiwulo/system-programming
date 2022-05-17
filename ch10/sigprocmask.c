#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

void sighandler(int signumber) {
    printf("get a signal named '%d', '%s'\n", signumber, strsignal(signumber));
}

/**
 * @brief 設定signal的「遮罩」，被「遮住」的signal會被作業系統「暫停分發」，
 * 如果發生了「被遮掉的signal」，那麼該signal的狀態會變成「pending」
 * 解開「遮罩」的時候，這個process會收到所有的pending signal
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */

int main(int argc, char **argv) {
    sigset_t sigset;
    assert(signal(SIGQUIT, sighandler) != SIG_ERR);
    /*終止所有的signal*/
    sigfillset(&sigset);
    sigprocmask(SIG_SETMASK, &sigset, NULL);
    /*睡10秒鐘，這段時間不會接收signal，因此signal頂多就是pending*/
    printf("sleep 10sec\n");
    for(int i=0; i<10; i++) {
    	sleep(1); write(1, "*", 1);
    }
    printf("\n");
    /*重新啓動所有的signal*/
    sigemptyset(&sigset);
    sigprocmask(SIG_SETMASK, &sigset, NULL);
    while (1) {
	    pause();
    }
}
