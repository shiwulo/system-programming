/*
usage: fork2
*/
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
int main()
{
    int var = 0;
    pid_t pid;
    //printf是在fork前面
    //printf("<<before fork>>");a
    pid = fork();
    if(pid == 0) {          /* child 執行 */
        var = 1;
	printf("child以死，黃天當立\n");
	sleep(1);
	
    } else if (pid > 0) {   /* parent 執行 */
	//signal(SIGCHLD, SIG_IGN);
	printf("my pid = %d\n", getpid());
        sleep(100);
    }
    printf("<<%d>>", var);
    return 0;
}
