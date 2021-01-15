#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv) {
	int fd;		//要寫出的檔案
	int num;	//計算列印到檔案多少字元了
	int printFileInfo=0;
	printf("『%s print』即時列印檔案訊息，不加參數則會列印\n", argv[0]);
	if (argc == 1) {
		printf("不列印檔案訊息\n");
		printFileInfo = 0;
	} else {
		printf("列印檔案訊息\n");
		printFileInfo = 1;
	}
	printf("按下enter後繼續\n");
	getchar();


	fd = open("./syncDataOnly.data",O_WRONLY | O_CREAT | O_TRUNC, 0644);
	for(num=0; num <=100000; num++) {
		write(fd, "1234", sizeof("1234"));
		//只同步data沒有同步metadata
		fdatasync(fd);
		if (num%10000==1) {
			write(1, "*", sizeof("*"));
			fsync(1);
		}
		if (num%100==0 && printFileInfo==1)
			system("ls syncDataOnly.data -alhs");
	}
	return 0;
}
