/*
usage: testStack
*/
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>

long long i;
int main(int argc, char** argv) {
    char a;
    struct rlimit rlim;
    //底下是獲得 stack 的大小，預設應該是 8MB
    getrlimit(RLIMIT_STACK, &rlim);
    printf("soft=%dK, hard=%dK\n", 
        (int)rlim.rlim_cur/1024, (int)rlim.rlim_max/1024);
    printf("pid = %d", getpid());
    //準備開始測試
    getchar();
    //測試方法是：
    // 1. 變數 a 在堆疊中
    // 2. 取得 a 的 address
    // 3. 堆疊由上往下長，因此從 a 的 address 開始，不斷的往下存取
    // 4. 超過OS預設的堆疊大小時，程式就死掉了，哈哈哈
    for (i=0; i<(8192 * 1024); i++) {
        if (i%1024 ==0)
            printf("%8lld kb\n",i/1024);
        *(&a - i) =1;
    }
    printf("sucess, i = %lld\n", i);
}