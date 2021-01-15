#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
 
#define BUF_SIZE 4096
 
int main(int argc, char* argv[]) {
 
    //從inputFd將檔案寫到outputFd
    int inputFd, outputFd;
    //讀進多少，寫出多少
    ssize_t numIn, numOut;
    //把檔案內容讀到buffer，再寫出去
    char buffer[BUF_SIZE];

    //確定使用者輸入二個參數
    if (argc != 3) {
        char* filename=basename(argv[0]);
        printf("『%s』的功能是檔案複製，要有二個參數，來源檔案和目標檔案\n", filename);
        exit(0);
    }
 
    //打開來源檔案
    inputFd = open (argv [1], O_RDONLY);
    if (inputFd == -1) {
        char* filename=basename(argv[1]);
        char errmsg[1024];
        sprintf(errmsg, "無法開啟來源檔案 (%s)", filename);
        perror (errmsg); 
        exit(1); 
    }
 
    //打開目的檔案
    //注意 open 的參數，可讀、創造、歸零（O_WRONLY | O_CREAT | O_TRUNC）
    //比較常忘記的是歸零，是否歸零是情況而定
    //如果沒有歸零就會有新舊混淆的問題
    //資料庫系統（DBMS）打開打檔案通常不會歸零，因為他們會在既有的上面做更新
    //word打開檔案通常要歸零，因為使用者的新文章可能更短，這會造成新舊混淆問題
    outputFd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR| S_IWUSR);
    if(outputFd == -1){
        char* filename=basename(argv[3]);
        char errmsg[1024];
        sprintf(errmsg, "無法打開目的檔案 (%s)", filename);
        perror (errmsg); 
        exit(1); 
    }
    
    //在while敘述中，將檔案的資料讀入，共毒入numIn個字元
    while((numIn = read (inputFd, buffer, BUF_SIZE)) > 0) {
        //將資料寫出到目的檔案
        numOut = write (outputFd, buffer, (ssize_t) numIn);
        //如果寫出的和讀取的數量不同，那就是有問題
        if(numIn != numOut) {
            perror("numIn != numOut"); exit(1);
        }
    }
    close (inputFd);
    close (outputFd);
    return (EXIT_SUCCESS);
}


