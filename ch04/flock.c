// 這是一個使用flock函數鎖定檔案的程式
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>

int main(int argc, char* argv[]) {
    int file_descriptor; // 檔案描述符
    int flock_return_value; // flock函數回傳值
    char lock_option; // 鎖定選項
    file_descriptor = open(argv[1], O_WRONLY); // 以寫入模式開啟檔案
    printf("已開啟檔案，描述符為：%d\n", file_descriptor);
    sscanf(argv[2], "%c", &lock_option); // 從輸入中取得鎖定選項
    switch (lock_option) {
        case 's':
            flock_return_value = flock(file_descriptor, LOCK_SH); // 共享鎖定
            break;
        case 'e':
            flock_return_value = flock(file_descriptor, LOCK_EX); // 獨佔鎖定
            break;
        case 'u':
            flock_return_value = flock(file_descriptor, LOCK_UN); // 釋放鎖定
            break;
        default:
            printf("輸入錯誤\n");
    }
    if (flock_return_value != 0) // 若鎖定失敗
        perror("flock");
    printf("已執行完畢\n");
    getchar();
    return 0;
}

