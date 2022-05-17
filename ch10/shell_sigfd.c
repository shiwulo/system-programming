/*執行方法：
./myShell
>> ls -R /
ctr-c
>> ls /home/shiwulo -R
*/
#include <assert.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
/// color///
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"
#define RED_BOLD "\x1b[;31;1m"
#define BLU_BOLD "\x1b[;34;1m"
#define YEL_BOLD "\x1b[;33;1m"
#define GRN_BOLD "\x1b[;32;1m"
#define CYAN_BOLD_ITALIC "\x1b[;36;1;3m"
#define RESET "\x1b[0;m"

/*
全域變數，放解析過後的使用者指令（字串陣列）
*/
char *argVect[256];

/*
parseString：將使用者傳進的命令轉換成字串陣列
str：使用者傳進的命令
cmd：回傳執行檔
*/
void parseString(char *str, char **cmd) {
  int idx = 0;
  char *retPtr;
  // printf("%s\n", str);
  retPtr = strtok(str, " \n");
  while (retPtr != NULL) {
    // printf("token =%s\n", retPtr);
    // if(strlen(retPtr)==0) continue;
    argVect[idx++] = retPtr;
    if (idx == 1)
      *cmd = retPtr;
    retPtr = strtok(NULL, " \n");
  }
  argVect[idx] = NULL;
}

// 🧡 💛 💚 💙 💜
// 設定要將哪些siganl轉乘signalfd
// 🧡 💛 💚 💙 💜
//設定要聽的singal到singalfd中
//回傳值是fd
int setupSignalfd() {
  sigset_t sigset;
  sigemptyset(&sigset);
  sigfillset(&sigset); //監聽所有可以監聽的signal
  sigaddset(&sigset, SIGINT);
  sigaddset(&sigset, SIGCHLD);
  //因為要用signalfd處理，因此就不要在觸發signal
  sigprocmask(SIG_BLOCK, &sigset, NULL);
  //-1代表要產生新的fd
  return signalfd(-1, &sigset, SFD_CLOEXEC);
}

//印出提示訊息
void printPrompt() {
  char cwd[256], hostName[256];
  char *showPath, *loginName;
  int homeLen = 0, pos = 0;

  //抓取主機名稱、用戶名稱
  loginName = getlogin();
  gethostname(hostName, 256);
  //底下程式碼製造要顯示的路徑字串，會參考"home"將"home"路徑取代為~
  getcwd(cwd, 256);
  pos = strspn(getenv("HOME"), cwd);
  homeLen = strlen(getenv("HOME"));
  // printf("cwd=%s, home=%s, pos=%d, prompt=%s", cwd, getenv("HOME"), pos,
  // &cwd[pos]);
  if (pos >= homeLen) {
    cwd[pos - 1] = '~';
    showPath = &cwd[pos - 1];
  } else
    showPath = cwd;
  //底下程式碼負責印出提示符號
  printf(LIGHT_GREEN "%s@%s:" BLU_BOLD "%s>> " NONE, loginName, hostName,
         showPath);
  //因為沒有換行符號，因此必須fflush，否則linebuffer情況下，不會立即印出東西
  fflush(stdout); 
}

/**
 * @brief 
 * 🧡 💛 💚 💙 💜
 * 這個程式最特別的地方是「使用signalfd」讀取signal，而非使用signal handler
 * 這樣做的好處是「不用像signal handler那樣，要額外想辦法和主程式同步」
 * 🧡 💛 💚 💙 💜
 * @param argc 
 * @param argv 
 * @return int 
 */

int main(int argc, char **argv) {
  char cmdLine[4096];
  char *exeName;
  int child_pid = -1, wstatus;        // wstatus紀錄child的結束狀態
  struct timespec startTime, endTime; //計算child的執行時間
  // for signal_fd
  int epollfd, sig_fd;
  struct epoll_event ev, event;
  struct signalfd_siginfo fdsi;

  printf(YELLOW "pid is %d\n", getpid()); //幫助除錯用
  // for debugging，可以開另外一個terminal，用(gdb)attach (gdb) b121
  // 睡10秒，有充分的時間設定breakpoint
  // sleep(10);

  //🧡 💛 💚 💙 💜
  //這裡空要使用二個技巧，一個是signal fd，另一個是epoll
  //epoll可以同時間看多個file descriptor
  //🧡 💛 💚 💙 💜
  //設定epoll
  sig_fd = setupSignalfd();
  epollfd = epoll_create1(0); //隨便傳一個參數進去都可以，Linux不管這個參數了
  // printf("epollfd %d\n", epollfd);
  ev.events = EPOLLIN; //監看指定的這群fd中，有任何一個可以read的時候
  ev.data.fd = STDIN_FILENO; //監聽鍵盤
  assert(epoll_ctl(epollfd, EPOLL_CTL_ADD, 1, &ev) != -1);
  ev.data.fd = sig_fd; //監聽聽signal
  assert(epoll_ctl(epollfd, EPOLL_CTL_ADD, sig_fd, &ev) != -1);

  /*無窮迴圈直到使用者輸入exit */
  while (1) {
    if (child_pid == -1)
      printPrompt();
  wait_event:
    //🧡 💛 💚 💙 💜 等待鍵盤或者是signal發生 🧡 💛 💚 💙 💜
    assert(epoll_wait(epollfd, &event, 1, -1) != -1);
    //🧡 💛 💚 💙 💜 由event得知到底是哪個file descriptor需要處理 🧡 💛 💚 💙 💜
    if (event.data.fd == sig_fd) { //收到signal
      int s =
          read(sig_fd, &fdsi,
               sizeof(struct signalfd_siginfo)); //用read讀取該signal的詳細資訊
      // assert(s==sizeof(struct signalfd_siginfo));   for debugging
      switch (fdsi.ssi_signo) { //判斷signal number
      case SIGINT:
        if (child_pid > 0) {
          int ret = kill(child_pid, fdsi.ssi_signo);
          if (ret == -1) {
            // system("touch kill_fault");   //for debugging
            perror("kill child");
            exit(1);
          }
          child_pid = -1;
          goto wait_event;
        } else {
          printf("\n");
        }
        break;
      case SIGCHLD: //假設child結束
        //印出執行時間、系統的時間、使用者時間
        clock_gettime(CLOCK_REALTIME, &endTime);
        printf(RED "real: " YELLOW "%ld.%ldsec\n",
               (endTime.tv_sec - startTime.tv_sec) +
                   (endTime.tv_nsec - startTime.tv_nsec) / 1000000000,
               ((endTime.tv_sec - startTime.tv_sec) * 1000000000 +
                (endTime.tv_nsec - startTime.tv_nsec)) %
                   1000000000);
        int HZ = sysconf(_SC_CLK_TCK); //取得utime, stime的單位
        // printf("resloution is %ldHZ\n",sysconf(_SC_CLK_TCK));     /for
        // debugging
        printf(RED "user: " YELLOW "%04ld.%ld sec\n", fdsi.ssi_utime / HZ,
               fdsi.ssi_utime % HZ);
        printf(RED "sys : " YELLOW "%04ld.%ld sec\n", fdsi.ssi_stime / HZ,
               fdsi.ssi_stime % HZ);

        //印出結束狀態
        wstatus = fdsi.ssi_status;
        if (WIFEXITED(wstatus)) { // child執行結束
          printf(RED "return value of " YELLOW "%s" RED " is " YELLOW "%d\n",
                 exeName, WEXITSTATUS(fdsi.ssi_status));
        } else if (WIFSIGNALED(wstatus)) { // child被signal打斷
          printf(RED "the child process was terminated by a signal " YELLOW
                     "%d" RED ", named " YELLOW "%s.\n",
                 WTERMSIG(fdsi.ssi_status),
                 strsignal(WTERMSIG(fdsi.ssi_status)));
        }
        printf(NONE);
        child_pid = -1;
        break;
      default: //印出不感興趣的signal名稱
        printf(RED "signal # is %d\n", fdsi.ssi_signo);
      }
      continue; //處理signal完以後，繼續接受使用者輸入
    }
    if (event.data.fd == STDIN_FILENO) { //來自鍵盤
      int ret = read(STDIN_FILENO, cmdLine, 4096);
      cmdLine[ret - 1] =
          '\0'; //因為read不會在字串後面加上'\0'，因此將'\n'換成'\0'，成為標準的C字串
      if (child_pid > 0)
        goto wait_event; //如果child正在執行，就暫時不處理使用者新的命令
    }
    // printf("cmdLine = %s\n",cmdLine);     //for debugging
    if (strlen(cmdLine) > 1) //判斷長度是否大於1，判斷「使用者無聊按下enter鍵」
      parseString(cmdLine, &exeName);
    else
      continue;
    if (strcmp(exeName, "exit") == 0) //內建指令exit
      break;
    if (strcmp(exeName, "cd") == 0) { //內建指令cd
      if (strcmp(argVect[1], "~") == 0)
        chdir(getenv("HOME"));
      else
        chdir(argVect[1]);
      continue;
    }
    child_pid = vfork(); //除了exit, cd，其餘為外部指令
    if (child_pid == 0) {
      clock_gettime(
          CLOCK_REALTIME,
          &startTime); //因為使用vfork，因此child更新startTime會寫到parent的記憶體空間
      //要記得打開signal的遮罩，因為execve會延續這個遮罩，如果沒有打開遮罩，那麼chlild可能會有些signal收不到
      sigset_t sigset;
      sigfillset(&sigset);
      sigprocmask(SIG_UNBLOCK, &sigset, NULL);
      //產生一個child執行使用者的指令
      if (execvp(exeName, argVect) == -1) {
        perror("myShell");
        exit(errno * -1);
      }
    }
  }
}
