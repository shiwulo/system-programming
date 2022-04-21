/*
usage: debugFork2
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;
    int waiting = 1;
    if ((pid = fork()) == 0) {
        //在child的地方加入一個空回圈，gdb attach成功以後再從gdb中，將waiting設定為 0
	    while (waiting)
            ;
	    printf("child");
    } else {
	    printf("child's pid = %d\n", pid);
	    printf("parent");
    }
    printf("end\n");
    return 0;
}
