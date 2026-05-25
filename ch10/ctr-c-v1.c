/*
 * 教學目的：
 *
 * 這個程式用來示範 UNIX/Linux 中 signal 的基本使用方式，
 * 特別是如何攔截使用者按下 Ctrl-C 時產生的 SIGINT。
 *
 * 一般情況下：
 *
 *   使用者按下 Ctrl-C
 *        ↓
 *   terminal driver 送出 SIGINT 給前景程式
 *        ↓
 *   程式預設會被中止
 *
 * 但是這個程式使用：
 *
 *   signal(SIGINT, sigint_handler);
 *
 * 將 SIGINT 的處理方式改成自訂的 sigint_handler()。
 *
 * 程式行為：
 *
 *   1. 第一次按 Ctrl-C：
 *        不立刻離開程式，而是印出：
 *
 *            Press Ctrl-C twice
 *
 *   2. 如果在 2 秒內再按一次 Ctrl-C：
 *        程式呼叫 exit(0)，直接結束。
 *
 *   3. 如果超過 2 秒才再按 Ctrl-C：
 *        重新當作第一次 Ctrl-C。
 *
 * 這可以讓學生理解：
 *
 *   - signal handler 是非同步執行的函式
 *   - signal handler 可以改變程式對 Ctrl-C 的反應
 *   - 程式可以用全域變數記錄 signal 發生的狀態
 *   - time() 與 difftime() 可用來判斷兩次事件的時間差
 *
 * 注意：
 *
 *   這是教學用程式。
 *
 *   嚴格來說，printf(), time(), difftime(), exit()
 *   都不適合直接放在 signal handler 裡。
 *
 *   正式系統程式中，signal handler 通常只設定
 *   volatile sig_atomic_t 型別的旗標，
 *   然後讓 main() 在正常控制流程中處理後續動作。
 */

#include <stdio.h>      /* printf() */
#include <stdlib.h>     /* exit() */
#include <signal.h>     /* signal(), SIGINT */
#include <unistd.h>     /* sleep() */
#include <time.h>       /* time(), time_t, difftime() */

/*
 * last_ctrlc：
 *
 * 記錄上一次按下 Ctrl-C 的時間。
 *
 * time_t 是 time() 回傳的型別，
 * 通常用來表示從 Unix epoch 到現在經過的秒數。
 *
 * 初始值為 0，表示目前還沒有記錄過 Ctrl-C 的時間。
 */
time_t last_ctrlc = 0;

/*
 * ctrlc_count：
 *
 * 用來記錄目前是否已經按過一次 Ctrl-C。
 *
 * 在這個程式中，它的用法比較像是一個狀態旗標：
 *
 *   ctrlc_count == 0
 *       表示目前還沒有有效的第一次 Ctrl-C。
 *
 *   ctrlc_count == 1
 *       表示使用者已經按過一次 Ctrl-C，
 *       接下來如果在 2 秒內再按一次，就結束程式。
 */
int ctrlc_count = 0;

/*
 * sigint_handler：
 *
 * 這是 SIGINT 的 signal handler。
 *
 * 當使用者按下 Ctrl-C 時，
 * terminal 會送出 SIGINT 給前景程式。
 *
 * 因為 main() 裡註冊了：
 *
 *   signal(SIGINT, sigint_handler);
 *
 * 所以程式收到 SIGINT 時，不會直接用預設行為結束，
 * 而是改為呼叫這個函式。
 *
 * signo：
 *   代表收到的 signal 編號。
 *   對 Ctrl-C 來說，通常 signo == SIGINT。
 *
 * 這個範例沒有使用 signo，
 * 所以加上 (void)signo; 避免 compiler warning。
 */
void sigint_handler(int signo)
{
    (void)signo;

    /*
     * 取得目前時間。
     *
     * time(NULL) 會回傳目前時間。
     * 單位通常是秒。
     *
     * 這裡用來判斷：
     *   這次 Ctrl-C 和上一次 Ctrl-C 是否相差 2 秒以內。
     */
    time_t now = time(NULL);

    /*
     * 判斷目前這次 Ctrl-C 是否應該被視為「第一次 Ctrl-C」。
     *
     * 條件一：
     *
     *   ctrlc_count == 0
     *
     * 表示之前還沒有按過 Ctrl-C，
     * 所以這次一定是第一次。
     *
     * 條件二：
     *
     *   difftime(now, last_ctrlc) > 2
     *
     * 表示距離上一次 Ctrl-C 已經超過 2 秒。
     * 即使之前按過 Ctrl-C，也要重新當作第一次。
     */
    if (ctrlc_count == 0 || difftime(now, last_ctrlc) > 2) {
        /*
         * 第一次 Ctrl-C 不結束程式。
         *
         * 只提示使用者：
         *   如果真的要離開，請在短時間內再按一次 Ctrl-C。
         */
        printf("Press Ctrl-C twice\n");

        /*
         * 記錄目前已經按過一次 Ctrl-C。
         */
        ctrlc_count = 1;

        /*
         * 記錄這一次 Ctrl-C 發生的時間。
         *
         * 下一次 Ctrl-C 會用這個時間來判斷是否在 2 秒內。
         */
        last_ctrlc = now;
    } else {
        /*
         * 如果程式執行到這裡，表示：
         *
         *   1. 使用者之前已經按過一次 Ctrl-C
         *   2. 這次 Ctrl-C 距離上一次沒有超過 2 秒
         *
         * 所以判定使用者真的想結束程式。
         *
         * exit(0)：
         *   結束程式，並回傳狀態碼 0 給 shell。
         *
         * 狀態碼 0 通常表示正常結束。
         */
        exit(0);
    }
}

int main(void)
{
    /*
     * 註冊 SIGINT 的處理函式。
     *
     * SIGINT：
     *   Interrupt signal。
     *   通常由使用者按 Ctrl-C 產生。
     *
     * signal(SIGINT, sigint_handler) 的意思是：
     *   之後程式收到 SIGINT 時，呼叫 sigint_handler()。
     */
    signal(SIGINT, sigint_handler);

    /*
     * 主程式持續執行。
     *
     * 這裡用無窮迴圈模擬一個長時間執行的程式，
     * 例如 server、daemon、監控程式或背景工作。
     */
    while (1) {
        /*
         * 每秒印出目前時間。
         *
         * 這可以讓學生觀察：
         *
         *   - 程式仍然持續執行
         *   - 第一次 Ctrl-C 不會中止程式
         *   - 第二次 Ctrl-C 才會結束程式
         */
        printf("%ld\n", time(NULL));

        /*
         * 暫停 1 秒。
         *
         * 如果沒有 sleep(1)，while 迴圈會高速空轉，
         * 造成 CPU 使用率接近 100%。
         */
        sleep(1);
    }

    /*
     * 理論上程式不會執行到這裡，
     * 因為 while (1) 是無窮迴圈。
     *
     * 不過保留 return 0 是良好的 main() 寫法。
     */
    return 0;
}
