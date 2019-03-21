#include <stdio.h>
int main() {
	FILE *stream;
	char buf[100];
	int ret;
	stream = fopen("./system-programming.txt", "a+");
	ret = fseek(stream, 200, SEEK_SET);
	printf("\nreturn value of fseek(stream, 200)= %d\n", ret);
	printf("file position = %ld\n", ftell(stream));
	fread(buf, 100, 1, stream);
	printf(buf);
	fprintf(stream, "append?\n");
	printf("\nfile position = %ld\n", ftell(stream));
	return 0;
}
