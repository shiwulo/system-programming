/*
使用方法：
不用給任何參數
*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
extern char **environ;

int main(int argc, char**argv) {
	int i=0;
	while(environ[i] != NULL) {
		printf("%s\n", environ[i++]);
	}
}
