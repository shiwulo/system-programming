// 這個程式示範了如何在 C 語言中使用標準 I/O 库對文件進行讀寫操作
#include <stdio.h>
#include <string.h>

int main() {
    FILE *stream;  // 宣告一個文件指針
    char buf[10000];  // 宣告一個字符數組作為緩衝區
    int ret;  // 定義一個整數變數 ret
    
    // 以讀寫方式打開一個文件，並獲取其文件指針
    stream = fopen("./system-programming.txt", "a+");
    
    // 使用 fseek 函數設置文件指針的位置，並輸出其返回值
    ret = fseek(stream, 10, SEEK_SET);
    printf("\nreturn value of fseek(stream, 10)= %d\n", ret);
    
    // 使用 ftell 函數獲取文件指針的位置，並輸出其返回值
    printf("file position after fseek(10) = %ld\n", ftell(stream));
    
    // 使用 memset 函數初始化緩衝區，並使用 fread 函數讀取文件內容到緩衝區
    memset(buf, 0, 10000);
    int nItem = fread(buf, 26, 1, stream);
    
    // 輸出從文件中讀取的內容
    printf("%s\n", buf);
    
    // 再次使用 ftell 函數獲取文件指針的位置，並輸出其返回值
    printf("file position after fread() = %ld\n", ftell(stream));
    
    // 使用 getchar 函數等待用戶輸入一個字符
    getchar();
    
    // 使用 fprintf 函數將一個字符串寫入文件
    fprintf(stream, "append?\n");
    
    // 再次使用 ftell 函數獲取文件指針的位置，並輸出其返回值
    printf("\nfile position = %ld\n", ftell(stream));
    
    // 關閉文件，並結束程式
    fclose(stream);
    return 0;
}
