#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

/* 
 * 功能：用指定的 uid 來執行 /bin/bash 指令
 * 用法：./program <uid>
 *      其中 <uid> 為要切換的使用者 ID
 * 備註：本程式必須以 root 權限執行
 */
int main(int argc, char** argv) {
    int uid;
	
    // 從命令行參數中讀取要切換的使用者 ID
    sscanf(argv[1], "%d", &uid);
    
    // 先以 root 權限執行 setuid(0)，再以指定 uid 切換使用者權限
    setuid(0);
    setuid(uid);
    
    // 執行 /bin/bash 指令
    system("/bin/bash");
    
    return 0;
}
