#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

/**
 * @brief 主程式停下來，直到等待的signal發生
 * 
 * @return int 
 */

int main() {
    sigset_t sigset;
    int signo;
    //設定為『等所有的signal』
    sigfillset(&sigset);
    //sigprocmask(SIG_SETMASK, &sigset, NULL);
    printf("pid = %d\n", getpid());
    while(1) {
        assert(sigwait(&sigset, &signo) == 0);
        printf("recv sig#%d\n", signo);
    }
}