#include <stdio.h>
#include <string.h>
int main() {
	FILE *stream;
	char buf[10000];
	int ret;
	stream = fopen("./system-programming.txt", "a+");
	ret = fseek(stream, 10, SEEK_SET);
	printf("\nreturn value of fseek(stream, 10)= %d\n", ret);
	printf("file position after fseek(10) = %ld\n", ftell(stream));
	memset(buf, 0, 10000);
	int nItem = fread(buf, 26, 1, stream);
	printf("%s\n", buf);
	printf("file position after fread() = %ld\n", ftell(stream));
	getchar();
	fprintf(stream, "append?\n");
	printf("\nfile position = %ld\n", ftell(stream));
	return 0;
}
