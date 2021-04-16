/**
DT_BLK      This is a block device. 「b」
DT_CHR      This is a character device. 「c」
DT_DIR      This is a directory. 「d」
DT_FIFO     This is a named pipe (FIFO). 「f」
DT_LNK      This is a symbolic link. 「l」
DT_REG      This is a regular file. 「-」
DT_SOCK     This is a UNIX domain socket. 「s」
DT_UNKNOWN  The file type could not be determined. 「U」
*/


#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <stddef.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>
//底下這個.h定義了最長的path name長度
#include <linux/limits.h>

//檔案型別應該不會超過100個
int filetype[100];
char fileSymbol[100];

//準備統計目錄中到底有多少種檔案型別，如果有該型別在filetype[X]上設定為1，該型別的代表字放在fileSymbol
void initFileTypoe() {
    for (int i=0; i< 99; i++) {
        filetype[i] = -1;
    }
    /*同學自己補下去*/
    fileSymbol[DT_BLK]='b';
    fileSymbol[DT_CHR]='c';
    fileSymbol[DT_DIR]='c';
}

//回傳某個檔案的大小
int readSize(char* pathname) {
    struct stat buf;
    //On success, zero is returned.  On error, -1 is returned, and errno is set appropriately.
    //https://blog.xuite.net/chowler/mainblog/5194764-assert%28%29+%E7%94%A8%E6%B3%95
    //assert()裡面填寫『我認為應該如此』，如果不是這樣的話，C函數庫會吐出錯誤訊息給programmer
    //在這裡用lstat和stat都可以，因為pathname傳進來的只會是normal file，不會是「捷徑」（softlink）
    printf("readSize:%s\n", pathname);
    assert(stat(pathname, &buf)==0);
    return buf.st_size;

}

//使用遞迴計算某個目錄中的所有正規檔案的大小，並統計到底有多少種檔案型別
//如果沒有權限打開該打檔案怎麼辦？
// man access
/*       access() checks whether the calling process can access the file path‐
       name.  If pathname is a symbolic link, it is dereferenced.

       The mode specifies the accessibility check(s) to be performed, and is
       either  the value F_OK, or a mask consisting of the bitwise OR of one
       or more of R_OK, W_OK, and X_OK.  F_OK tests for the existence of the
       file.   R_OK,  W_OK, and X_OK test whether the file exists and grants
       read, write, and execute permissions, respectively.
*/
long myCountDir(char* path) {
    long size = 0; 
    //打開該目錄
    DIR* dirp = opendir(path);
    //讀取該目錄的第一個「物件」
    struct dirent* ent = readdir(dirp);
    while (ent != NULL) {
        //『這個目錄』及『上一層目錄』跳過不處理
        if (strcmp(ent->d_name, "." )==0 || strcmp(ent->d_name, ".." )==0) {
            ent = readdir(dirp);
            continue;
        }
        //設定有這種檔案型別
        filetype[ent->d_type] = 1;
        //製造『路徑/名』
        //如果使用者的輸入是「/」怎麼辦？，例如：「//home」會發生錯誤嗎？
        char pathname[PATH_MAX]="";
        strcat(pathname, path);
        strcat(pathname, "/");
        strcat(pathname, ent->d_name);
        printf("%s", pathname);
        //如果是目錄
        if (ent->d_type == DT_REG) {
            //遞迴呼叫
            size += readSize(pathname);
        } else if (ent->d_type == DT_DIR) {
            printf("myCountDir:%s\n", pathname);
            size += myCountDir(pathname);  
        }
        ent = readdir(dirp);
    }
    closedir(dirp);
    return size;
}

int main(int argc, char** argv) {
    initFileTypoe();
    myCountDir(argv[1]);
}
