#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief 在這個程式中假設作業系統最多有100個signal，
 * 然後每一個signal都去攔截看看
 * 這個程式要和send_sig搭配使用
 * 
 */

int nSig[100];

//所有的signal都使用這一個signal handler接收
void sighandler(int signumber) {
    //統計某一種signal收集了多少個
    nSig[signumber]++;
}

int main(int argc, char **argv) {
    //得知某一個signal是否可以「註冊」
    //特別注意，有些行程管理用的signal是不能註冊的，例如SIGKILL
    int sig_exist[100];
    int idx = 0;
    for (idx = 0; idx < 100; idx++) {
        if (signal(idx, sighandler) == SIG_ERR) {
            sig_exist[idx] = 0;
        } else {
            sig_exist[idx] = 1;
        }
    }
    printf("my pid is %d\n", getpid());
    printf("現在這一個process已經可以開始接收signal了\n");
    printf("press any key to count the signal number\n");
    getchar();
    //列印某種signal各自接收了多少個
    for (idx=0; idx<100; idx++) {
        if (nSig[idx] != 0)
            printf("signal #%d, %d times\n", idx, nSig[idx]);
    }
    
    return 0;
}


