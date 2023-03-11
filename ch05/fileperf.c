// 這個程式是一個文本處理程式，用於將輸入文件中的單詞以指定的方式格式化輸出到輸出文件中
// 可以使用命令行參數指定輸入文件名、輸出文件名以及緩衝模式
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv)
{
    FILE *input = fopen(argv[1], "r");  // 以只讀方式打開輸入文件，並獲取其文件指針
    FILE *output = fopen(argv[2], "w");  // 以寫入方式打開輸出文件，並獲取其文件指針

    int num = atoi(argv[3]);  // 將第三個命令行參數轉換為整數，並賦值給變數 num

    if (num == 0)
    { //unbuffered
        printf("unbuffered\n");
        assert(setvbuf(input, NULL, _IONBF, 0)==0);  // 使用 setvbuf 函數將輸入文件設置為無緩衝模式
        assert(setvbuf(output, NULL, _IONBF, 0)==0);  // 使用 setvbuf 函數將輸出文件設置為無緩衝模式
    }
    else if (num == -1)
    { //linebuffered
        printf("linebuffered\n");
        assert(setvbuf(input, NULL, _IOLBF, 0)==0);  // 使用 setvbuf 函數將輸入文件設置為行緩衝模式
        assert(setvbuf(output, NULL, _IOLBF, 0)==0);  // 使用 setvbuf 函數將輸出文件設置為行緩衝模式
    }
    else
    { //fullybuffered
        printf("fullybuffered\n");
        int buff_size = num;
        assert(setvbuf(input, NULL, _IOFBF, buff_size) == 0);  // 使用 setvbuf 函數將輸入文件設置為全緩衝模式
        assert(setvbuf(output, NULL, _IOFBF, buff_size) == 0);  // 使用 setvbuf 函數將輸出文件設置為全緩衝模式
    }

    char word_buf[82];  // 宣告一個字符數組作為單詞的緩衝區，長度為最大單詞長度加上空格
    int buflen = 0;  // 定義一個整數變數 buflen，表示單詞緩衝區中已經寫入的字符數
    int linePos = 0;  // 定義一個整數變量 linePos，表示在輸出文件中當前行中已經寫入的字符數
    int total = 0; // 定義一個整數變數 total，表示在輸出文件中已經寫入的總字符數
    while (1)
    {
        int inputInt = getc(input);  // 從輸入文件中讀取一個字符的 ASCII 編碼
        if (inputInt == EOF)  // 如果已經到達了輸入文件的結尾，則退出循環
            break;
        unsigned char inputChar = (unsigned char)inputInt;  // 將讀取到的整數轉換為 unsigned char 型別
        if (inputChar == '\n') {  // 如果讀取到的字符為換行符，則在輸出文件中插入一個換行符
            fprintf(output, "\n");
            linePos=0;
        }
        if (inputChar == ' ')
        {  // 如果讀取到的字符為空格，則將單詞緩衝區中的內容格式化輸出到輸出文件中
            if (linePos + buflen > 80)  // 如果單詞緩衝區中的內容加上當前行已經寫入的內容超過了 80 個字符，則在輸出文件中插入一個換行符
            {
                fprintf(output, "\n");
                total += linePos;
                linePos = 0;
            }
            word_buf[buflen] = ' ';  // 將空格寫入單詞緩衝區中
            word_buf[buflen+1] = '\0';  // 在單詞緩衝區中加入字符串結束符
            fprintf(output, "%s", word_buf);  // 將單詞緩衝區中的內容寫入輸出文件中
            linePos += buflen+1;  // 更新當前行中已經寫入的字符數
            buflen = 0;  // 清空單詞緩衝區
            continue;
        }
        word_buf[buflen++] = inputChar;  // 如果讀取到的字符不是空格或換行符，則將其寫入單詞緩衝區中
    }
    return 0;
}
