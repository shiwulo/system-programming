// 這個程式示範了在 C 語言中使用 strlen 函數獲取一個字串的長度
#include <stdio.h>
#include <wchar.h>
#include <string.h>

int main(int argc, char **argv)
{
    char *str = "中文";  // 宣告一個字符指針 str，並賦值為 "中文"
    printf("%d\n", (int)strlen(str));  // 使用 strlen 函數獲取 str 的長度，並輸出其返回值
    return 0;
}


