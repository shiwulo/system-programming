#include <stdio.h>
#include <dirent.h>

/* 
 * 功能：列出指定目錄中的所有檔案和子目錄名稱
 * 用法：./program <目錄名稱>
 */
int main(int argc, char **argv)
{
    DIR* dir;
    struct dirent* ent;
    
    // 打開目錄
    dir = opendir(argv[1]);
    
    // 讀取目錄中的所有檔案和子目錄
    ent = readdir(dir);
    while(ent!=NULL) {
        printf("%s\n", ent->d_name);
        ent = readdir(dir);
    }
    
    // 關閉目錄
    closedir(dir);
	
    return 0;
}


