#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

volatile char *c; 

void sighandler(int signumber) {
    printf("get a signal named '%d', '%s'\n", signumber,
           sys_siglist[signumber]);
    c=malloc(sizeof(char)); //這一行到底有沒有用，要看編譯器怎樣編譯
}

/**
 * @brief 一個很簡單程式，故意存取錯誤的記憶體然後攔截這個錯誤。
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */

int main(int argc, char **argv) {
    assert(signal(SIGSEGV, sighandler) != SIG_ERR);  
    *c = 'a';   //雖然在signal handler的地方重新給 c 值，但 *c 其實已經放在暫存器（register）中，因此signal handler無法修正這個錯誤
    printf("my pid is %d\n", getpid());
    printf("press any key to resume\n");
    getchar();
    return 0;
}


