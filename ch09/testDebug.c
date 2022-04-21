#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;
    if ((pid = fork()) == 0) {
	    printf("child's pid = %d\n", getpid());
    } else {
	    printf("parent's pid = %d\n", getpid());
    }
    //可以在下一行設定中斷點
    printf("breakpoint\n");
    if (pid ==0) {
        //照理說只有child會執行下面這一行，我們之前已經設定中斷點了，gdb可以追蹤到這一行嗎？
        //應該無法，這是因為gdb預設追蹤parent
        //如果要讓他預設追蹤child的話
        //請用google 搜尋 『gdb child process』
        system("touch child");
    }
    return 0;
}
