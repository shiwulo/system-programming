#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
 
int main(int argc, char* argv[]) {
 
    int inputFd, outputFd;
    char *inputPtr, *outputPtr;
    ssize_t numIn, numOut;
    ssize_t fileSize=0;
    //char buffer[BUF_SIZE];
 
    inputFd = open (argv [1], O_RDONLY);
    if (inputFd == -1) {
        perror ("cannot open the file for read"); exit(1); }
 
    outputFd = open(argv[2], O_RDWR | O_CREAT, S_IRUSR| S_IWUSR);
    if(outputFd == -1){
        perror("canot open the file for write"); exit(1); }

    fileSize = lseek(inputFd, 0, SEEK_END);

    printf("file size = %ld\n", fileSize);

    inputPtr = mmap(NULL, fileSize, PROT_READ, MAP_SHARED , inputFd , 0);
    perror("mmap");
    printf("inputPtr = %p\n", inputPtr);
    //assert(madvise(inputPtr, fileSize, MADV_SEQUENTIAL|MADV_WILLNEED|MADV_HUGEPAGE)==0);

    ftruncate(outputFd, fileSize);
    outputPtr = mmap(NULL, fileSize, PROT_WRITE, MAP_SHARED , outputFd , 0);
    perror("mmap, output");
    printf("outputPtr = %p\n", outputPtr);
    //madvise(inputPtr, fileSize, MADV_SEQUENTIAL|MADV_WILLNEED|MADV_HUGEPAGE);

    printf("memory copy\n");
    time_t timer1, timer2;
    timer1 = time(NULL);
    memcpy(outputPtr, inputPtr, fileSize);
    timer2 = time(NULL);

    printf("time(memcpy) = %ld sec \n", timer2 - timer1);

    assert(munmap(inputPtr, fileSize) == 0);
    assert(munmap(outputPtr, fileSize) == 0);
    
    assert(close (inputFd) == 0);
    assert(close (outputFd) == 0);

    return (EXIT_SUCCESS);
}