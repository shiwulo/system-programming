/*
使用ChatGPT修正
這個程式主要的改進是：
1. 使用寬字符集（wide character）處理中文等非ASCII字符，避免中文變成亂碼。
2. 將單詞寬度計算部分改成一個函數，以支持不同的字符寬度計算方式。
3. 修復了一些可能導致程式異常終止或者輸出錯誤的漏洞和錯誤。
4. 將讀取字符的方式改為使用 getwc 函數，以便在處理寬字符集時正確讀取字符。
5. 修改了一些變量名稱和函數名稱，以使其更具可讀性和可維護性。
6. 針對每一個函數都添加了注釋，以便理解其作用和實現方式。
儘管這個程式在處理非ASCII字符方面更為健壯，但在某些極端情況下可能仍然會有問題。如果需要處理更複雜的文字處理任務，建議使用更先進的文字處理庫或工具。
*/

#define _XOPEN_SOURCE		/* See feature_test_macros(7) */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#define MAX_WORD_LENGTH 80	// 定義單詞的最大長度

// 計算一個 Unicode 字符的寬度，包括 ASCII 字符、制表符和中文等多字節字符
int get_wchar_width(wchar_t wc)
{
    if (wc == L'\t') {
	return 8;		// 假設制表符寬度為 8
    } else if (wc == L'\n' || wc == L'\r') {
	return 0;		// 换行符和回车符的宽度为 0
    } else if (wc >= 0x20 && wc <= 0x7E) {
	return 1;		// ASCII 字符寬度為 1
    } else {
	return wcwidth(wc);	// 其他字符寬度由庫函數計算
    }
}

int main(int argc, char *argv[])
{
	
    if (argc != 3) {
	fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
	exit(EXIT_FAILURE);
    }

    setlocale(LC_ALL, "");	// 設置環境為當前語言
	
/**

    char *input_file = NULL; // 輸入文件名稱，預設為 NULL
    char *output_file = NULL; // 輸出文件名稱，預設為 NULL

    int opt;
    while ((opt = getopt(argc, argv, "i:o:")) != -1) { // 使用 getopt 函数解析命令行参数
        switch (opt) {
        case 'i': // 如果是选项 i，则 optarg 指向对应的参数字符串
            input_file = optarg;
            break;
        case 'o': // 如果是选项 o，则 optarg 指向对应的参数字符串
            output_file = optarg;
            break;
        default: // 如果是未知选项，则返回 '?'，并输出提示信息
            fprintf(stderr, "Usage: %s [-i input_file] [-o output_file]\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (input_file == NULL || output_file == NULL) { // 如果缺少输入或输出文件，则输出提示信息
        fprintf(stderr, "Usage: %s [-i input_file] [-o output_file]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *input_fp = fopen(input_file, "r");	// 打開輸入文件
    if (!input_fp) { // 如果打开文件失败，则输出错误信息
        fprintf(stderr, "Failed to open input file '%s'\n", input_file);
        exit(EXIT_FAILURE);
    }

    FILE *output_fp = fopen(output_file, "w");	// 打開輸出文件
    if (!output_fp) { // 如果打开文件失败，则输出错误信息
        fprintf(stderr, "Failed to open output file '%s'\n", output_file);
        exit(EXIT_FAILURE);
    }

*/

    FILE *input_file = fopen(argv[1], "r");	// 打開輸入文件
    if (!input_file) {
	fprintf(stderr, "Failed to open input file '%s'\n", argv[1]);
	exit(EXIT_FAILURE);
    }

    FILE *output_file = fopen(argv[2], "w");	// 打開輸出文件
    if (!output_file) {
	fprintf(stderr, "Failed to open output file '%s'\n", argv[2]);
	exit(EXIT_FAILURE);
    }

    int line_width = 0;		// 紀錄當前行中已經輸出的字符寬度
    int total_width = 0;	// 紀錄單詞總寬度
    bool need_space = false;	// 是否需要在單詞之前插入一個空格
    wchar_t word_buf[MAX_WORD_LENGTH + 1];	// 單詞緩衝區
    while (1) {
	wint_t c = fgetwc(input_file);	// 從輸入文件中讀取一個 Unicode 字符
	if (c == WEOF) {
	    break;		// 如果到達文件結尾，則退出循環
	}
	if (c == L'\n') {
	    if (fputwc(L'\n', output_file) == WEOF) {
		fprintf(stderr,
			"Failed to write newline to output file\n");
		exit(EXIT_FAILURE);
	    }
	    line_width = 0;	// 在輸出文件中插入換行符
	} else if (c == L' ' || c == L'\t') {
	    if (total_width > 0) {
		word_buf[total_width] = L'\0';	// 在單詞緩衝區中加入字符串結束符
		int word_width = 0;
		for (int i = 0; i < total_width; i++) {
		    word_width += get_wchar_width(word_buf[i]);	// 計算單詞寬度
		}
		if (line_width + word_width + (need_space ? 1 : 0) >
		    MAX_WORD_LENGTH) {
		    if (fputwc(L'\n', output_file) == WEOF) {
			fprintf(stderr,
				"Failed to write newline to output file\n");
			exit(EXIT_FAILURE);
		    }
		    line_width = 0;	// 如果單詞寬度超過 80，則在輸出文件中插入換行符
		}
		if (need_space) {
		    if (fputwc(L' ', output_file) == WEOF) {
			fprintf(stderr,
				"Failed to write space to output file\n");
			exit(EXIT_FAILURE);
		    }
		    line_width++;	// 如果需要插入空格，則在輸出文件中插入一個空格
		}
		if (fputws(word_buf, output_file) == WEOF) {
		    fprintf(stderr,
			    "Failed to write word to output file\n");
		    exit(EXIT_FAILURE);
		}
		line_width += word_width;
		total_width = 0;	// 清空單詞緩衝區和相關變量
	    }
	    need_space = false;
	} else {
	    if (total_width == MAX_WORD_LENGTH) {
		fprintf(stderr, "Word is too long\n");
		exit(EXIT_FAILURE);
	    }
	    word_buf[total_width++] = c;	// 如果讀取到的字符不是換行符、空格或制表符，則加入單詞緩衝區
	    need_space = true;
	}
    }
    if (total_width > 0) {	// 處理最後一個單詞
	word_buf[total_width] = L'\0';
	int word_width = 0;
	for (int i = 0; i < total_width; i++) {
	    word_width += get_wchar_width(word_buf[i]);
	}
	if (line_width + word_width + (need_space ? 1 : 0) >
	    MAX_WORD_LENGTH) {
	    if (fputwc(L'\n', output_file) == WEOF) {
		fprintf(stderr,
			"Failed to write newline to output file\n");
		exit(EXIT_FAILURE);
	    }
	    line_width = 0;
	}
	if (need_space) {
	    if (fputwc(L' ', output_file) == WEOF) {
		fprintf(stderr, "Failed to write space to output file\n");
		exit(EXIT_FAILURE);
	    }
	    line_width++;
	}
	if (fputws(word_buf, output_file) == WEOF) {
	    fprintf(stderr, "Failed to write word to output file\n");
	    exit(EXIT_FAILURE);
	}
	line_width += word_width;
    }

    if (fclose(input_file) == EOF) {
	fprintf(stderr, "Failed to close input file\n");
	exit(EXIT_FAILURE);
    }
    if (fclose(output_file) == EOF) {
	fprintf(stderr, "Failed to close output file\n");
	exit(EXIT_FAILURE);
    }

    printf("Total %d characters output to %s\n", line_width, argv[2]);
}
