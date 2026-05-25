/*
 * 教學目的：
 *
 * 這個程式用來示範 UNIX/Linux signal 的幾個重要概念：
 *
 * 1. 當使用者按下 Ctrl-C 時，終端機會送出 SIGINT 給前景程式。
 *
 * 2. 程式用 signal(SIGINT, sigint_handler) 註冊 SIGINT handler，
 *    因此 Ctrl-C 不會使用預設行為直接結束程式，而是改由 sigint_handler()
 *    來處理。
 *
 * 3. 在 signal handler 執行期間，如果使用者再次按下 Ctrl-C，
 *    同一個 SIGINT 通常不會立刻重入 handler，而是可能變成 pending signal。
 *
 * 4. sigpending() 可以查詢目前有哪些 signal 正在等待遞送。
 *    sigismember(&pending, SIGINT) 可以檢查 SIGINT 是否在 pending set 裡。
 *
 * 5. signal handler 不直接呼叫 exit() 結束程式，而是設定 exit_flag。
 *    主程式在 while 迴圈中看到 exit_flag 被設定後，再跳出迴圈並做收尾。
 *
 * 注意：
 *
 * 這是教學用程式，刻意把一些觀察訊息放在 signal handler 裡。
 * 嚴格來說，printf(), time(), difftime(), sigpending(), sigismember()
 * 都不是最理想的 signal-handler 內部操作。
 *
 * 在正式系統程式中，signal handler 應該盡量只做非常簡單的事情，
 * 例如設定 volatile sig_atomic_t 旗標，然後讓主程式處理後續工作。
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

/*
 * exit_flag：
 *
 * 這個變數由 signal handler 設定，由 main() 讀取。
 *
 * 因為它會在非同步的 signal handler 中被修改，
 * 所以使用 volatile sig_atomic_t。
 *
 * sig_atomic_t 的意思是：
 *   對這個型別的讀寫，在 signal handler 和主程式之間是比較安全的基本單位。
 *
 * volatile 的意思是：
 *   要求 compiler 不要把這個變數的讀取最佳化掉。
 *   因為它可能在 main() 看不到的地方，也就是 signal handler 裡被改變。
 */
volatile sig_atomic_t exit_flag = 0;

/*
 * nCtrC：
 *
 * 記錄目前已經進入 SIGINT handler 幾次。
 *
 * 注意：
 * 這個數字記錄的是 handler 被執行的次數，
 * 不一定等於使用者實際按 Ctrl-C 的次數。
 *
 * 如果 handler 正在執行，而 SIGINT 被暫時 block，
 * 使用者多按幾次 Ctrl-C，可能只會留下 pending 狀態，
 * 不一定會讓 handler 每次都立刻執行。
 */
volatile sig_atomic_t nCtrC = 0;

/*
 * SIGINT 的 signal handler。
 *
 * signo 是收到的 signal number。
 * 對 Ctrl-C 而言，通常 signo == SIGINT。
 */
void sigint_handler(int signo)
{
    /*
     * 這裡沒有使用 signo。
     * 加上這行可以避免 compiler 產生 unused parameter warning。
     */
    (void)signo;

    /*
     * 記錄 handler 被呼叫的次數。
     *
     * 第一次按 Ctrl-C 時，nCtrC 會從 0 變 1。
     */
    nCtrC++;

    /*
     * 教學用輸出：
     *
     * 表示程式已經進入 SIGINT handler，
     * 正在模擬「取消資料」或「清理狀態」的過程。
     *
     * 注意：
     * printf() 嚴格來說不適合放在 signal handler 裡。
     * 這裡是為了教學觀察方便。
     */
    printf("\n資料正在取消中，請稍候\n");

    /*
     * 這個 for 迴圈模擬 signal handler 需要花 5 秒處理事情。
     *
     * 在這 5 秒內，使用者可以嘗試再次按 Ctrl-C。
     *
     * 教學觀察重點：
     *   如果在 handler 執行期間再次按 Ctrl-C，
     *   後續的 SIGINT 可能不會立刻執行 handler，
     *   而是進入 pending 狀態。
     */
    for (int i = 1; i <= 5; i++) {
        sigset_t pending;
        int isPending = 0;

        /*
         * sigpending(&pending)
         *
         * 取得目前 thread 中「已經產生，但尚未被遞送」的 signal 集合。
         *
         * 回傳值：
         *   0  表示成功
         *  -1  表示失敗
         */
        if (sigpending(&pending) == 0) {
            /*
             * sigismember(&pending, SIGINT)
             *
             * 檢查 SIGINT 是否在 pending signal set 裡。
             *
             * 回傳值：
             *   1  表示 SIGINT 正在 pending
             *   0  表示 SIGINT 沒有 pending
             *  -1  表示錯誤
             */
            isPending = sigismember(&pending, SIGINT);
        }

        /*
         * 模擬 handler 裡正在做一秒鐘的工作。
         *
         * 這裡故意使用 busy waiting，讓 handler 停留較久，
         * 方便學生在 handler 執行期間再按 Ctrl-C。
         *
         * 注意：
         * busy wait 會浪費 CPU。
         * 正式程式不應該這樣寫。
         */
        time_t loop_start = time(NULL);

        while (difftime(time(NULL), loop_start) < 1) {
            /*
             * busy wait: teaching only
             *
             * 教學用途：
             *   故意讓 signal handler 卡住一段時間，
             *   觀察這段期間再次按 Ctrl-C 時，SIGINT 是否變成 pending。
             */
        }

        /*
         * 印出目前觀察結果。
         *
         * nCtrC：
         *   目前這是第幾次進入 Ctrl-C handler。
         *
         * i：
         *   handler 裡模擬處理的第幾秒。
         *
         * isPending：
         *   1 表示目前有 SIGINT 正在 pending。
         *   0 表示目前沒有 SIGINT pending。
         *
         * 實驗方式：
         *   執行程式後按一次 Ctrl-C。
         *   當它顯示「資料正在取消中，請稍候」後，
         *   在 5 秒內再按幾次 Ctrl-C。
         *   觀察 isPending 是否變成 1。
         */
        printf("第 %d 次 Ctrl-C，第 %d 秒，有 %d 個 SIGINT 在等待\n",
               (int)nCtrC, i, isPending);
    }

    /*
     * 模擬取消失敗。
     *
     * 這裡不直接呼叫 exit()。
     * 而是設定 exit_flag = 1，通知 main() 應該結束。
     */
    printf("取消失敗，準備離開\n");

    exit_flag = 1;
}

int main(void)
{
    /*
     * 註冊 SIGINT handler。
     *
     * 當使用者按 Ctrl-C 時，系統會送出 SIGINT。
     * 因為這裡註冊了 sigint_handler，
     * 所以 SIGINT 會交給 sigint_handler() 處理。
     */
    signal(SIGINT, sigint_handler);

    /*
     * 主程式正常工作迴圈。
     *
     * 只要 exit_flag 還是 0，就繼續執行。
     *
     * exit_flag 會在 sigint_handler() 裡被設定成 1。
     */
    while (!exit_flag) {
        /*
         * 這裡代表主程式正在做其他工作。
         *
         * 使用 sleep(1) 是為了避免空轉吃滿 CPU。
         * 如果這裡寫成 while (!exit_flag) { }
         * CPU 使用率會接近 100%。
         */
        sleep(1);
    }

    /*
     * 程式從 main() 的正常流程離開。
     *
     * 教學重點：
     *   signal handler 不直接結束程式。
     *   signal handler 只設定旗標。
     *   main() 看到旗標後，進行資源釋放與正常收尾。
     */
    printf("從主程式離開，而不是從 signal handler 直接離開\n");

    return 0;
}
