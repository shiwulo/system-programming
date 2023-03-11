// 這個程式創建了一個名為 tmp 的文件，並在其中寫入 10000000 個字母 d，使用 setvbuf 函數將文件的緩衝區設置為指定大小，並使用 malloc 函數動態分配內存空間
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    FILE* stream;
    int bufSize;
    int dataSize = 10000000;
    char *buf;
    int i;
    
    stream = fopen("./tmp", "w+");  // 創建一個文件，文件名為 tmp，以讀寫方式打開，並獲取其文件指針
    sscanf(argv[1], "%d", &bufSize);  // 從命令行參數中讀取緩衝區大小，並將其轉換為整數
    buf = (char*)malloc(bufSize);  // 使用 malloc 函數動態分配內存空間，大小為緩衝區大小
    setvbuf(stream, buf, _IOFBF, bufSize);  // 使用 setvbuf 函數將文件的緩衝區設置為指定大小，並指定分配的內存空間
    // 在文件中寫入 10000000 個字母 d
    for (i=0; i<dataSize; i++)
        fwrite("d", 1, 1, stream);  
    return 0;
}

