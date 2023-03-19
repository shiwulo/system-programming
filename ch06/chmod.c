#include <stdio.h>
#include <sys/stat.h>

/* 
 * 功能：設定檔案權限
 * 用法：./program <檔案名稱> <三位數權限碼>
 *      其中 <三位數權限碼> 為三位數數字，表示檔案的所有者、群組、其他人權限
 * 備註：本程式必須以 root 權限執行
 */
int main(int argc, char **argv)
{
    int perm;
    int owner, grp, others;
    
    // 從命令行參數中讀取要設定權限的檔案名稱和三位數權限碼
    sscanf(argv[2], "%1d%1d%1d", &owner, &grp, &others);
    
    // 將三位數權限碼合併為一個八進制數字
    perm = owner << 6 | grp << 3 | others;
    
    // 使用 chmod 函數設定檔案權限
    chmod(argv[1], (mode_t)perm);
	
    return 0;
}
