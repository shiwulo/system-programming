#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

/* 
 * 功能：列出指定目錄中的所有檔案和子目錄名稱，以及它們的詳細資訊
 * 用法：./program <目錄名稱>
 */
int main(int argc, char **argv) {
    DIR* dir;
    struct dirent* ent;
    char* curDir = "./";
    char pathname[512];
    struct stat buf;
    int perm;
    char *time;
	
    // 打開目錄
    dir = opendir(argv[1]);
    
    // 讀取目錄中的所有檔案和子目錄
    ent = readdir(dir);
    while(ent!=NULL) {
        // 構造檔案的完整路徑名稱
        strcpy(pathname, curDir);
        strcat(pathname, ent->d_name);
        
        // 使用 stat 函數讀取檔案的詳細資訊
        stat(pathname, &buf); 
        perm = (buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
        time = ctime(&buf.st_atime);
        time[strlen(time)-1] = 0;
        
        // 輸出檔案的詳細資訊
        printf("%o  %d  %d %8d %s %s\n", perm, buf.st_uid, buf.st_gid, (int)buf.st_size, time, ent->d_name);
        
        ent = readdir(dir);
    }
    
    // 關閉目錄
    closedir(dir);
    
    return 0;
}
