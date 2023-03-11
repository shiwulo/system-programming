// 這個程式示範了如何在 C 語言中使用標準 I/O 库對文件進行讀取操作
#include <stdio.h>

int main() {
    FILE *stream;  // 宣告一個文件指針
    char buf[5000];  // 宣告一個字符數組作為緩衝區
    int ret;  // 定義一個整數變數 ret
    
    // 以讀寫方式打開一個文件，並獲取其文件指針
    stream = fopen("./tmp", "a+");
    
    // 使用 fread 函數從文件中讀取 500 個字節到緩衝區 buf 中，並輸出其返回值
    ret = fread(buf, 10, 500, stream);
    printf("ret = %d\n", ret);
    
    // 再次使用 fread 函數從文件中讀取 500 個字節到緩衝區 buf 中，並輸出其返回值
    ret = fread(buf, 10, 500, stream);
    printf("ret = %d\n", ret);
    
    // 如果返回值不為 1，則判斷讀取是否出現錯誤或到達了文件結尾，並輸出對應信息
    if(ret!=1) {
        if (ferror(stream))
            printf("error\n");
        if (feof(stream))
            printf("EOF\n");
    }
    
    // 關閉文件，並結束程式
    fclose(stream);
    return 0;
}

