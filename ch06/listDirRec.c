/**
功能：列出目錄中所有的檔案和子目錄，並以樹狀結構顯示。
用法：./a.out directory_path
*/

#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <linux/limits.h>

// 儲存目前的深度，用於縮排
int level = 0;

// 列出檔案或子目錄的函數
void printName(char* type, char* name) {
    printf("%s", type); // 印出檔案型別
    for (int i = 0; i < level; i++) {
        printf("  "); // 縮排
    }
    if (strcmp("d", type) == 0) {
        printf("+"); // 表示目錄的符號
    } else {
        printf("|"); // 表示檔案的符號
    }
    printf("%s\n", name);
}

// 列出目錄中所有檔案和子目錄的函數
void listDir(char* pathName)
{
    level++; // 每進入一層目錄，深度加一
    DIR* curDir = opendir(pathName); // 開啟目錄
    assert(curDir != NULL); // 確認目錄存在
    char* newPathName = (char*)malloc(PATH_MAX); // 記憶體分配
    struct dirent entry;
    struct dirent* result;
    struct direct* ret;
    result = readdir(curDir); // 讀取目錄中的內容
    while (result != NULL) {
        if (strcmp(entry.d_name, ".") == 0 || strcmp(entry.d_name, "..") == 0) { // 跳過「.」和「..」
            result = readdir(curDir);
            assert(ret == 0);
            continue;
        }
        assert(ret == 0);
        if (entry.d_type == DT_LNK) // 印出軟連結
            printName("l", entry.d_name);
        if (entry.d_type == DT_REG) // 印出檔案
            printName("f", entry.d_name);
        if (entry.d_type == DT_DIR) { // 印出目錄，並遞迴呼叫
            printName("d", entry.d_name);
            sprintf(newPathName, "%s/%s", pathName, entry.d_name);
            printf("%s\n", newPathName);
            listDir(newPathName);
        }
        result = readdir(curDir);
        assert(ret == 0);        
    }
    closedir(curDir); // 關閉目錄
    level--; // 回到上一層目錄，深度減一
}

int main(int argc, char** argv) {
    listDir(argv[1]); // 列出目錄中的所有檔案和子目錄
}
