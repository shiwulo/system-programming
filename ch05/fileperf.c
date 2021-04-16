#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv)
{
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");

    int num = atoi(argv[3]);
    if (num == 0)
    { //unbuffered
        printf("unbuffered\n");
        assert(setvbuf(input, NULL, _IONBF, 0)==0);
        assert(setvbuf(output, NULL, _IONBF, 0)==0);
    }
    else if (num == -1)
    { //linebuffered
        printf("linebuffered\n");
        assert(setvbuf(input, NULL, _IOLBF, 0)==0);
        assert(setvbuf(output, NULL, _IOLBF, 0)==0);
    }
    else
    { //fullybuffered
//根據man setvbuf 的解釋，系統會自動設定malloc然後設定buffer
/*
Except for unbuffered files, the buf argument should 
point to a buffer at least size bytes long;  this
buffer  will be used instead of the current buffer.  
If the argument buf is NULL, only the mode is af‐
fected; a new buffer will be allocated on the next read 
or write operation.
*/
        printf("unbuffered\n"); 
        int buff_size = num;
        //char *buf1 = (char *)malloc(buff_size);
        //char *buf2 = (char *)malloc(buff_size);
        assert(setvbuf(input, NULL, _IOFBF, buff_size) == 0);
        assert(setvbuf(output, NULL, _IOFBF, buff_size) == 0);
    }

    //假設一個字的最大長度是80個字母
    char word_buf[82];
    //word_buf已經用了多少個字母
    int buflen = 0;
    //在螢幕上已經列印到第幾個字母
    int linePos = 0; //already write in the line
    int total = 0;
    while (1)
    {
        int inputInt = getc(input);
        if (inputInt == EOF)
            break;
        unsigned char inputChar = (unsigned char)inputInt;
        if (inputChar == '\n') {
            fprintf(output, "\n");
            linePos=0;
        }
        if (inputChar == ' ')
        {
            if (linePos + buflen > 80)
            {
                fprintf(output, "\n");
                total += linePos;
                //printf("total:%d\t", total);
                linePos = 0;
            }
            word_buf[buflen] = ' ';
            //printf("%d, ", buflen);
            word_buf[buflen+1] = '\0';
            fprintf(output, "%s", word_buf);
            linePos += buflen+1;
            //printf("buflen:%d\t", buflen);
            buflen = 0;
            continue;
        }
        word_buf[buflen++] = inputChar;
    }
    return 0;
}
