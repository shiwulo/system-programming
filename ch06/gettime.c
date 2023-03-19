/*
 * gettime.c
 * 功能：顯示當前系統時間
 * 用法：./program
 */
#include <stdio.h>
#include <time.h>
 
int main(void)
{
    // 獲取當前時間
    time_t t = time(NULL);
    
    // 使用 localtime 函數將時間轉換為 struct tm 格式
    struct tm *pTM = localtime(&t);
    
    // 獲取年、月、日、時、分、秒等時間元素
    int yyyy = pTM->tm_year + 1900;
    int mm = pTM->tm_mon + 1;
    int dd = pTM->tm_mday;
    int hh = pTM->tm_hour;
    int min = pTM->tm_min;
    int ss = pTM->tm_sec;
     
    // 輸出格式化後的時間
    printf("%d-%02d-%02d-%02d:%02d:%02d\n", yyyy, mm, dd, hh, min, ss);
}
