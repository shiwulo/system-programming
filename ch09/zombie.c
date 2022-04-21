/*
usage:
1st terminal 
$ ./zombie 10000

2nd terminal
$ ps -a
*/
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int i, num, pid, wstatus;
    sscanf(argv[1], "%d", &num);
    //底下這個loop會製造很多個child，
    for (int i=0; i<num; i++) {
        pid = vfork();
        //下面這一行代表child一出生就馬上死掉
        if (pid==0) exit(0);
        if (pid != 0) continue;
    }
    //parent製造完child以後，parnet躺平不幹事
    //這時候可以用 ps -aux 觀察系統中的process的狀態
    //會發現很多 process 變成 zombie
    if (pid != 0)
        getchar();
}