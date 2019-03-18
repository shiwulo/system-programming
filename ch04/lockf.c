#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char* argv[]) {
	int fd;
	int ret;
	char opt;
	off_t begin, end;
	
	if (argc == 1) printf("lockf [file] [type(l/u)] [begin] [end]\n");

	fd = open (argv [1], O_WRONLY);
	printf("fd = %d is opened\n", fd);
	
	sscanf(argv[2], "%c", &opt);
	sscanf(argv[3], "%ld", &begin);
	sscanf(argv[3], "%ld", &end);

	switch (opt) {
		case 'l':
			lseek(fd, begin, SEEK_SET);
			ret = lockf(fd, F_LOCK, end - begin + 1);
			break;
		case 'u':
			lseek(fd, begin, SEEK_SET);
			ret = lockf(fd, F_UNLCK, end - begin + 1);
			break;
		default:
			printf("input error\n");
	}
	if (ret != 0)
		perror("flock");
	printf("end\n");
	getchar();
	return 0;
}
