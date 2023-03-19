#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>

/* 
 * 功能：列出當前進程的實際用戶 ID、有效用戶 ID 和保存的設定 ID
 * 用法：./program
 */
int main(int argc, char** argv) {
    uid_t ruid, euid, suid;
    
    // 使用 getresuid 函數獲取當前進程的實際用戶 ID、有效用戶 ID 和保存的設定 ID
    getresuid(&ruid, &euid, &suid);
    
    // 輸出當前進程的用戶 ID 以及對應的用戶名稱
    printf("ruid = %d, euid = %d, suid = %d\n", ruid, euid, suid);
    printf("ruid = %s, ", getpwuid(ruid)->pw_name);
    printf("euid = %s, ", getpwuid(euid)->pw_name);
    printf("suid = %s\n", getpwuid(suid)->pw_name);
}

