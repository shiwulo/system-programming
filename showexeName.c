#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	//Both dirname() and basename() return pointers to 
	//null-terminated strings. (Do not pass these pointers to free(3).
	char exeBuf[4096];
	char* exeName = basename(argv[0]);
	printf("argc = %d\n", argc);
	printf("%s\n", argv[0]);
	printf("%s\n", exeName);
	snprintf(exeBuf, 4096, "%s", exeName);
	for (int i=1; i< argc; i++) {
		printf("argv[%d] = %s\n", i, argv[i]);
		snprintf(&exeBuf[strlen(exeBuf)], 4096-strlen(exeBuf), " %s", argv[i]);
	}
	printf("%s", exeBuf);
	return system(exeBuf);
}
