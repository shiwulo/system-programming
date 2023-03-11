// 這個程式創建了一個名為 tmp 的文件，並在其中寫入一行文字，打開方式由命令行參數指定
#include <stdio.h>

int main(int argc, char **argv) {
    FILE* file;
    file = fopen("./tmp", argv[1]);  // 創建一個文件，文件名為 tmp，打開方式由命令行參數指定，並獲取其文件指針
    fprintf(file, "this_is_a_tmp_file\n");  // 在文件中寫入一行文字
    fclose(file);  // 關閉文件
    return 0;
}
