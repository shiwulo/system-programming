// 這個程式主要示範了一些關於 Unicode 字串在 C 語言中的操作
// 包括使用 wchar_t 定義 Unicode 字串、使用 fwide 函數判斷標準輸出流的編碼方案、以及使用 wcslen 函數獲取 Unicode 字串的長度
#include <stdio.h>
#include <wchar.h>

int main(int argc, char **argv)
{
    // 使用 wchar_t 定義一個 Unicode 字串 wstr
    wchar_t *wstr = L"中文"; 
    
    // 使用 fwide 函數判斷標準輸出流的編碼方案，並輸出其返回值
    printf("%d\n",fwide(stdout, 0));
    
    // 使用 wprintf 函數輸出 Unicode 字串 wstr
    wprintf(L"%lc\n",wstr);
    
    // 再次使用 fwide 函數判斷標準輸出流的編碼方案，並輸出其返回值
    printf("%d",fwide(stdout, 1));
    
    // 使用 wcslen 函數獲取 Unicode 字串 L"好" 的長度，並輸出其返回值
    printf("strlen(好) = %d\n",(int)wcslen(L"好"));
    
    // 再次使用 fwide 函數判斷標準輸出流的編碼方案，並輸出其返回值
    printf("%d",fwide(stdout, 0));
    
    // 輸出 "end"，並結束程式
    printf("end\n");
    return 0;
}

