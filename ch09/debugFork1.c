/*
usage debugFork1
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;
    if ((pid = fork()) == 0) {
	    printf("child\n");
    } else {
        //child執行得太快了，很難去使用 gdb attach 
	    printf("child's pid = %d\n", pid);
	    printf("parent\n");
    }
    printf("end\n");
    return 0;
}
