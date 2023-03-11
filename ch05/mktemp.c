// 這個程式創建了一個以隨機文件名命名的文件，並在其中寫入一行文字
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main() {
    FILE* stream;
    char tmpStr[] = "./shiwulo_XXXXXX";  // 宣告一個字符數組，用於存放隨機文件名
    mktemp(tmpStr);  // 使用 mktemp 函數生成一個隨機文件名，並將其保存在字符數組 tmpStr 中
    printf("%s\n", tmpStr);  // 在控制台輸出隨機文件名
    stream = fopen(tmpStr, "w+");  // 創建一個以隨機文件名命名的文件，以讀寫方式打開，並獲取其文件指針
    if (stream == NULL)
        perror("error: ");  // 如果打開文件失敗，則在控制台輸出錯誤信息
    fputs("hello\0", stdout);  // 在標準輸出中輸出一行文字
    return 0;
}

