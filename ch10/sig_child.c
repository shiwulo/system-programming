/*
 * 教學目的：
 *
 * 這個程式用來示範 UNIX/Linux 中 child process 結束後的回收問題，
 * 特別是 zombie process 與 SIGCHLD 的關係。
 *
 * 重要觀念：
 *
 * 1. fork() 會建立 child process。
 *
 * 2. child process 結束時，kernel 不會立刻完全移除它。
 *    kernel 會保留一小部分 process 資訊，例如：
 *      - process ID
 *      - exit status
 *      - resource usage summary
 *
 *    這些資訊要等 parent process 呼叫 wait() 或 waitpid() 之後才會被回收。
 *
 * 3. 如果 child 已經結束，但是 parent 還沒有 wait() 它，
 *    這個 child process 就會進入 zombie 狀態。
 *
 *    在 ps 中，zombie process 的狀態通常會顯示為 Z。
 *
 * 4. SIGCHLD 是 child process 狀態改變時，kernel 送給 parent 的 signal。
 *    最常見的情況是 child process 結束時，parent 會收到 SIGCHLD。
 *
 * 5. 這個程式使用：
 *
 *        signal(SIGCHLD, SIG_IGN);
 *
 *    表示 parent 忽略 SIGCHLD。
 *
 *    在許多 UNIX/Linux 系統上，這會使 child process 結束後被自動回收，
 *    因此 parent 不需要明確呼叫 wait()，也通常不會留下 zombie。
 *
 * 6. 程式最後用 ps 指令查詢：
 *
 *        目前 parent process 底下，有多少 child process 是 zombie。
 *
 * 實驗方式：
 *
 *   gcc -Wall -Wextra -o no_zombie no_zombie.c
 *   ./no_zombie
 *
 * 預期結果：
 *
 *   Zombies: 0
 *
 * 延伸實驗：
 *
 *   可以把 signal(SIGCHLD, SIG_IGN); 註解掉，
 *   然後在 fork 完之後加 sleep(10);
 *   再用另一個 terminal 執行 ps 觀察 zombie process。
 *
 * 注意：
 *
 *   這是教學用程式。
 *   正式程式中，更常見、也更可控的方式是使用 wait() 或 waitpid()
 *   主動回收 child process。
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int main(void)
{
    /*
     * N 表示要建立多少個 child process。
     *
     * 這裡設定成 100，是為了讓現象比較容易觀察。
     * 每個 child process 一建立後就會立刻結束。
     */
    const int N = 100;

    /*
     * 取得目前這個 process 的 PID。
     *
     * 這個 process 是 parent process。
     *
     * 後面會用這個 PID 去查詢：
     *   哪些 process 的 PPID 等於這個 parent 的 PID。
     *
     * 也就是查詢這個 parent 的 child process。
     */
    pid_t ppid = getpid();

    /*
     * 設定 SIGCHLD 的處理方式為 SIG_IGN。
     *
     * SIGCHLD：
     *   child process 結束、停止或繼續執行時，
     *   kernel 會送給 parent process 的 signal。
     *
     * SIG_IGN：
     *   表示忽略這個 signal。
     *
     * 教學重點：
     *
     *   一般情況下，child 結束後 parent 需要呼叫 wait() 或 waitpid()
     *   來回收 child 的 exit status。
     *
     *   如果 parent 沒有回收，child 會暫時變成 zombie。
     *
     *   但是在 Linux 上，如果 parent 明確設定 SIGCHLD 為 SIG_IGN，
     *   child 結束後通常會被 kernel 自動回收，
     *   因此不會留下 zombie process。
     */
    signal(SIGCHLD, SIG_IGN);

    /*
     * 建立 N 個 child process。
     *
     * 每次 fork() 之後，會產生兩個執行流程：
     *
     *   parent process：
     *     fork() 回傳 child 的 PID，值大於 0。
     *
     *   child process：
     *     fork() 回傳 0。
     *
     *   fork() 失敗：
     *     回傳 -1。
     */
    for (int i = 0; i < N; i++) {
        pid_t pid = fork();

        /*
         * fork() 失敗。
         *
         * 可能原因包括：
         *   - 系統 process 數量限制
         *   - 使用者 process 數量限制
         *   - 記憶體或 kernel resource 不足
         */
        if (pid < 0) {
            perror("fork");
            exit(1);
        }

        /*
         * pid == 0 表示目前執行的是 child process。
         *
         * child process 在這裡立刻結束。
         *
         * 使用 _exit(0)，而不是 exit(0)。
         *
         * 原因：
         *   exit() 會執行 stdio buffer flush、atexit handler 等使用者層清理工作。
         *   fork() 之後的 child 在這個範例中不需要做這些事情。
         *
         *   _exit() 會直接請 kernel 結束 process，
         *   比較適合 fork() 後立刻結束的 child。
         */
        if (pid == 0) {
            _exit(0);
        }

        /*
         * 如果 pid > 0，表示目前仍然是 parent process。
         *
         * parent 不呼叫 wait() 或 waitpid()。
         *
         * 這是刻意設計的：
         *   我們想觀察在 signal(SIGCHLD, SIG_IGN) 的情況下，
         *   child 結束後是否會留下 zombie。
         */
    }

    /*
     * 建立一個 shell command。
     *
     * 這個指令會檢查：
     *
     *   在目前 parent process 底下，
     *   有多少 child process 處於 zombie 狀態。
     *
     * ps -o pid,ppid,state,cmd --ppid <parent-pid>
     *
     *   -o pid,ppid,state,cmd
     *     指定 ps 輸出的欄位：
     *       pid    process ID
     *       ppid   parent process ID
     *       state  process 狀態
     *       cmd    執行的命令
     *
     *   --ppid <parent-pid>
     *     只列出 parent PID 等於 <parent-pid> 的 process。
     *
     * grep -c ' Z '
     *
     *   計算狀態欄中包含 Z 的列數。
     *
     *   Z 表示 zombie process。
     *
     * xargs echo 'Zombies:'
     *
     *   把計算結果印成：
     *
     *       Zombies: 0
     *
     * 注意：
     *   這裡使用 system() 是為了教學方便。
     *   正式程式通常不建議為了檢查 process 狀態而組 shell command。
     */
    char cmd[256];

    snprintf(cmd, sizeof(cmd),
             "ps -o pid,ppid,state,cmd --ppid %d | grep -c ' Z ' | "
             "xargs echo 'Zombies:'",
             (int)ppid);

    /*
     * 執行上面組好的 shell command。
     *
     * 預期在 Linux 上通常會看到：
     *
     *   Zombies: 0
     *
     * 因為 SIGCHLD 被設定為 SIG_IGN，
     * child process 結束後會被自動回收。
     */
    system(cmd);

    /*
     * main() 正常結束。
     */
    return 0;
}
