#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

int global_a;
int global_b=10;
int main()
{
	int local_c=10;
	int local_d;
	int *malloc_e = malloc(100);
	printf("印出指標的位址，因為ASLR的關係，每次印出來的結果會不一樣\n");
	printf(" global_a\t%p\n global_b\t%p\n local_c\t%p\n local_d\t%p\n malloc_e\t%p\n",
		&global_a, &global_b, &local_c, &local_d, malloc_e);
	char command[1024];
	printf("按任意按鍵，印出這個task的memory layout\n");
	getchar();
	sprintf(command, "pmap %d", getpid());
	assert(system(command)==0);
}
