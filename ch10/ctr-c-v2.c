/*
 * 教學目的：
 *
 * 這個程式用來示範 UNIX/Linux signal handling 中一個比較好的設計模式：
 *
 *     signal handler 不直接結束程式，
 *     而是設定一個旗標，
 *     讓 main() 在正常控制流程中偵測旗標並結束。
 *
 * 程式行為：
 *
 *   1. 程式每秒印出目前時間。
 *
 *   2. 使用者第一次按 Ctrl-C 時：
 *        - terminal 送出 SIGINT 給程式。
 *        - sigint_handler() 被呼叫。
 *        - 程式記錄這次 Ctrl-C 的時間。
 *        - 程式不會立刻結束。
 *
 *   3. 如果使用者在 2 秒內第二次按 Ctrl-C：
 *        - sigint_handler() 設定 stop = 1。
 *        - main() 的 while 迴圈下一次檢查到 stop == 1。
 *        - main() 跳出迴圈。
 *        - main() 印出「釋放資源，優雅地結束」。
 *        - 程式正常結束。
 *
 *   4. 如果第二次 Ctrl-C 距離第一次超過 2 秒：
 *        - 重新視為第一次 Ctrl-C。
 *        - 不會結束程式。
 *
 * 這個範例的教學重點：
 *
 *   - Ctrl-C 會產生 SIGINT。
 *   - signal(SIGINT, sigint_handler) 可以攔截 Ctrl-C。
 *   - signal handler 可以通知 main()，但不一定要直接結束程式。
 *   - volatile sig_atomic_t 適合用來在 signal handler 和 main() 之間傳遞簡單旗標。
 *   - 「設定結束旗標」比「在 handler 中直接 exit()」更適合做優雅結束。
 *
 * 注意：
 *
 *   這仍然是教學用程式。
 *
 *   嚴格來說，time() 和 difftime() 並不是 async-signal-safe，
 *   因此正式系統程式不建議在 signal handler 裡呼叫它們。
 *
 *   這裡保留這個寫法，是為了讓學生清楚觀察：
 *
 *       兩次 Ctrl-C 是否發生在 2 秒內。
 *
 *   更嚴謹的 production 寫法，通常會讓 handler 只設定旗標，
 *   時間判斷與輸出訊息則放在 main() 的正常流程中處理。
 */

#include <stdio.h>      /* printf() */
#include <signal.h>     /* signal(), SIGINT, sig_atomic_t */
#include <unistd.h>     /* sleep() */
#include <time.h>       /* time(), time_t, difftime() */

/*
 * last_ctrlc：
 *
 * 記錄上一次 Ctrl-C 發生的時間。
 *
 * time(NULL) 會回傳目前時間，型別是 time_t。
 * 這個值通常表示從 Unix epoch 到目前經過的秒數。
 *
 * 初始值設為 0，表示還沒有記錄過 Ctrl-C。
 */
time_t last_ctrlc = 0;

/*
 * ctrlc_count：
 *
 * 用來表示目前是否已經收到過一次有效的 Ctrl-C。
 *
 * 在這個程式中，它其實比較像狀態旗標：
 *
 *   ctrlc_count == 0
 *       尚未收到第一次 Ctrl-C。
 *
 *   ctrlc_count == 1
 *       已經收到第一次 Ctrl-C。
 *       如果 2 秒內再收到第二次 Ctrl-C，就準備結束程式。
 *
 * 注意：
 *
 *   這個變數也會在 signal handler 中被修改。
 *   若要更嚴謹，也可以宣告為 volatile sig_atomic_t。
 *
 *   這裡保留 int，是為了貼近原始教學程式。
 */
int ctrlc_count = 0;

/*
 * stop：
 *
 * 這是 signal handler 與 main() 之間溝通用的旗標。
 *
 *   stop == 0
 *       main() 繼續執行。
 *
 *   stop == 1
 *       main() 應該跳出 while 迴圈，準備結束。
 *
 * 為什麼使用 volatile sig_atomic_t？
 *
 *   1. sig_atomic_t：
 *        C 標準保證這種型別適合在 signal handler 中做簡單讀寫。
 *
 *   2. volatile：
 *        告訴 compiler 這個變數可能在 main() 看不到的地方被修改，
 *        也就是可能被 signal handler 非同步修改。
 *
 *        因此 compiler 不應該把 while 裡對 stop 的讀取最佳化掉。
 */
volatile sig_atomic_t stop = 0;

/*
 * sigint_handler：
 *
 * 這是 SIGINT 的 signal handler。
 *
 * 當使用者按下 Ctrl-C 時，terminal driver 會送出 SIGINT
 * 給目前前景執行中的 process。
 *
 * 因為 main() 中註冊了：
 *
 *     signal(SIGINT, sigint_handler);
 *
 * 所以收到 SIGINT 時，不採用預設的「直接中止程式」行為，
 * 而是改為執行這個函式。
 */
void sigint_handler(int signo)
{
    /*
     * signo 是收到的 signal number。
     *
     * 對 Ctrl-C 來說，通常 signo == SIGINT。
     *
     * 這個程式沒有實際使用 signo，
     * 因此用 (void)signo; 避免 compiler 產生 unused parameter warning。
     */
    (void)signo;

    /*
     * 取得目前時間。
     *
     * 教學目的：
     *   用來比較這次 Ctrl-C 與上一次 Ctrl-C 是否相差超過 2 秒。
     *
     * 注意：
     *   time() 嚴格來說不建議放在 signal handler 中。
     *   這裡是教學簡化版本。
     */
    time_t now = time(NULL);

    /*
     * 判斷這次 Ctrl-C 是否應該被視為「第一次 Ctrl-C」。
     *
     * 條件一：
     *
     *     ctrlc_count == 0
     *
     *   表示程式目前尚未記錄過第一次 Ctrl-C。
     *
     * 條件二：
     *
     *     difftime(now, last_ctrlc) > 2
     *
     *   表示距離上一次 Ctrl-C 已經超過 2 秒。
     *   即使之前按過一次 Ctrl-C，也重新當作第一次。
     *
     * 所以：
     *
     *   - 第一次 Ctrl-C：
     *       只記錄時間，不結束。
     *
     *   - 超過 2 秒後再按 Ctrl-C：
     *       重新當作第一次 Ctrl-C。
     */
    if (ctrlc_count == 0 ||
        difftime(now, last_ctrlc) > 2) {

        /*
         * 記錄「已經收到第一次 Ctrl-C」。
         *
         * 下一次 SIGINT 進來時，如果時間差沒有超過 2 秒，
         * 就會進入 else 分支，把 stop 設為 1。
         */
        ctrlc_count = 1;

        /*
         * 記錄這一次 Ctrl-C 的時間。
         */
        last_ctrlc = now;
    } else {
        /*
         * 執行到這裡表示：
         *
         *   1. 之前已經收到過一次 Ctrl-C。
         *   2. 這次 Ctrl-C 距離上一次沒有超過 2 秒。
         *
         * 因此判定使用者真的想結束程式。
         *
         * 不在 signal handler 中直接 exit()。
         * 只設定 stop = 1。
         *
         * 這就是此範例的核心教學重點：
         *
         *   handler 只通知 main()，
         *   main() 負責真正離開程式。
         */
        stop = 1;
    }
}

int main(void)
{
    /*
     * 註冊 SIGINT handler。
     *
     * SIGINT：
     *   Interrupt signal。
     *   通常由使用者按 Ctrl-C 產生。
     *
     * signal(SIGINT, sigint_handler)：
     *   告訴系統：之後本程式收到 SIGINT 時，
     *   請呼叫 sigint_handler()。
     */
    signal(SIGINT, sigint_handler);

    /*
     * 主程式工作迴圈。
     *
     * 這裡用 while (1) 模擬一個長時間執行的程式，
     * 例如：
     *
     *   - server
     *   - daemon
     *   - 背景工作程式
     *   - 長時間資料處理程式
     *
     * 每一輪迴圈都檢查 stop。
     * 如果 signal handler 已經把 stop 設為 1，
     * main() 就跳出迴圈。
     */
    while (1) {
        /*
         * 檢查 signal handler 是否要求程式結束。
         *
         * 第一次 Ctrl-C 不會設定 stop。
         * 只有 2 秒內的第二次 Ctrl-C 才會設定 stop = 1。
         */
        if (stop == 1)
            break;

        /*
         * 模擬程式正在正常工作。
         *
         * 這裡每秒印出目前時間。
         *
         * 教學觀察：
         *
         *   - 第一次 Ctrl-C 後，程式仍然繼續印時間。
         *   - 2 秒內第二次 Ctrl-C 後，程式不會直接死在 handler 裡，
         *     而是回到 main()，由 main() 跳出迴圈。
         */
        printf("%ld\n", time(NULL));

        /*
         * 暫停 1 秒，避免 while 迴圈空轉吃滿 CPU。
         */
        sleep(1);
    }

    /*
     * 跳出 while 後，程式回到 main() 的正常控制流程。
     *
     * 在真實程式中，這裡可以放：
     *
     *   - 關閉檔案
     *   - 關閉 socket
     *   - flush log
     *   - 釋放記憶體
     *   - 通知其他 thread/process
     *   - 儲存尚未完成的狀態
     *
     * 這就是「優雅地結束」的意思。
     */
    printf("釋放資源，優雅地結束\n");

    /*
     * main() 回傳 0，表示程式正常結束。
     */
    return 0;
}
