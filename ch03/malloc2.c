#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

int main() {
	int i;
	char* p;
	char command[1024];
	printf("執行malloc『前』的memory的layout\n");
	sprintf(command, "pmap %d", getpid());
	assert(system(command) >=0);
	printf("malloc 64*4K\n");
	for (i=0; i<64*4096; i++)
		p=(char*)malloc(1);
	printf("執行malloc『後』的memory的layout\n");
	sprintf(command, "pmap %d", getpid());
	assert(system(command) >=0);
}
