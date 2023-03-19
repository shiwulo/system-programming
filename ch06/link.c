#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv)
{
    int ret;

    // 確認參數是否符合預期
    printf("%d \n", argc);
    if (argc == 4) { /* 3個參數，建立符號連結 */
        ret = symlink(argv[2], argv[3]);
        printf("%s, %s\n", argv[2], argv[3]);

        // 確認是否建立成功，如果不成功則輸出錯誤訊息
        if (ret != 0)
            perror("soft link:");
    } else { /* 2個參數，建立硬連結 */
        ret = link(argv[1], argv[2]);
        printf("%s, %s\n", argv[1], argv[2]);

        // 確認是否建立成功，如果不成功則輸出錯誤訊息
        if (ret != 0)
            perror("hard link");
    }

    return 0;
}

