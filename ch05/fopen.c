// 這個程式創建了一個名為 tmp 的文件，並在其中寫入一行文字
#include <stdio.h>

int main(int argc, char **argv)
{
    FILE* file;
    file = fopen("./tmp", "w");  // 創建一個文件，文件名為 tmp，以寫入方式打開，並獲取其文件指針
    fprintf(file, "this is a tmp file\n");  // 在文件中寫入一行文字
    return 0;
}


