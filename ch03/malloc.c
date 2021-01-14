#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

int main() {
    char* p1;
    char* p2;
	char command[1024];
    /*印出行程的pid，方便我們到/proc目錄裡面找到相對映的檔案*/
    //printf("pid = %d\n", getpid());
    printf("malloc(64)\n");
    /*配置64byte記憶體*/
	p1 = (char*)malloc(64);
	printf("p1=%p\n", p1);
	printf("malloc 64*4K\n");
	/*配置256K記憶體*/
	p2 = (char*)malloc(64*4096);
	printf("p2=%p\n", p2);
	sprintf(command, "pmap %d", getpid());
	assert(system(command) >=0);
}



