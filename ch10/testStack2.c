//gcc decSP4K.s getSP.s -o testStack
//usage testStack mem|reg

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <alloca.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>

void seg_fault(int sigNo) {
	while(1);
}

/**
 * @brief 這個程式碼是不斷的修改SP（stack pointer），藉此讓堆疊變大，Linux預設stack最大只有8MB
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */

int main(int argc, char** argv) {
	char base;
	char *stackP=&base;
	int i;
	struct rlimit rlim;
	int usingSP;
	if (signal(SIGSEGV, seg_fault)==SIG_ERR) {
		perror("SIGSEGV");
	}

	if (argc != 2) exit(-1);
	if (strcmp("reg", argv[1]) ==0)
		usingSP = 1;
	else usingSP = 0;

	getrlimit(RLIMIT_STACK, &rlim);
	printf("soft=%dK, hard=%dK\n",
		(int)rlim.rlim_cur/1024, 
		(int)rlim.rlim_max/1024);
	printf("pid = %d", getpid());
	getchar();
	// 在底下這個while loop中不斷的 rsp 暫存器 減去 (4096-8)
	// 因為堆疊是由上往下長，因此相當於堆疊不斷的往下長
	// 底下有二種寫法，分別使用組合語言，另一個則是使用一個區域變數的address
	// 把local variable的address當作是堆疊的開始位址，然後往下移動4096 bytes
	while(1) {
		if(usingSP) {
			__asm__(
				"SUB $(4096-8), %rsp;\n"
			);
			asm (
				"MOVQ %%rsp, %0;\n"
				:"=r"(stackP)
			);
		} else {
			stackP -= 4096;
		}
		*stackP = 1;
		printf("%ldK\n",(&base - stackP)/(1024));
	}
}