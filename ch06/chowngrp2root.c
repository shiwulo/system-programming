#include <sys/types.h>
#include <unistd.h>

/* 
 * 功能：將指定檔案的所有者和群組設為 root
 * 用法：sudo ./program <檔案名稱>
 * 備註：本程式必須以 root 權限執行
 */
int main(int argc, char** argv)
{
    // 使用 chown 函數將檔案的所有者和群組都設為 root
    chown(argv[1], 0, 0);
    
    return 0;
}
