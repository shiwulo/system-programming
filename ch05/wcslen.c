// 這個程式使用寬字符集(wchar_t)輸出一個中文字符串的長度
#include <stdio.h>
#include <wchar.h>
#include <string.h>

int main(int argc, char **argv)
{
    wchar_t* wstr = L"中文";  // 宣告一個寬字符集字符串 wstr，並將其初始化為中文字符串
    printf("%d\n", (int)wcslen(wstr));  // 輸出寬字符集字符串的長度，即中文字符串的長度
    return 0;
}

