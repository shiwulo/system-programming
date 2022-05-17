/*執行方法：
 ./init
 >> ls -R /
 ctr-c
 */
#include <fcntl.h>           /* Definition of AT_* constants */
#include <stdio.h>
#include <dirent.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <setjmp.h>
#include <sys/resource.h>
///color///
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
char* argVect[256];

//jumpbuffer，setjmp和longjmp使用。類似於書籤，只能跳到「caller」
sigjmp_buf jumpBuf;
//shell是否產生child process，決定ctr-c要送給誰
volatile sig_atomic_t hasChild = 0;
//child的process id
pid_t childPid;
//每秒鐘有多少個nanoseconds
const long long nspersec = 1000000000;

long long timespec2sec(struct timespec ts) {
    long long ns = ts.tv_nsec;
    ns += ts.tv_sec * nspersec;
    return ns;
    //return (double)ns/1000000000.0;
}

//將timeval資料結構轉換成多少個 nanoseconds
double timeval2sec(struct timeval input) {
    long long us = input.tv_sec*1000000;
    us += input.tv_usec;
    //printf("%ldsec, %ld us\n", input.tv_sec, input.tv_usec);
    return (double)us/1000000.0;
}

/*signal handler專門用來處理ctr-c*/
void ctrC_handler(int sigNumber) {
    //如果shell有child，結束掉child。例如：正在執行ls
    if (hasChild) {
        kill(childPid, sigNumber);
        hasChild = 0;
    }
    //否則的話，先將「^c」放回到 input stream，然後讓主迴圈決定怎樣處理「^c」 
    else {
        /*確認main function並不是剛好在處理字串，這裡使用一個隱含的同步方法*/
        /*藉由確認是否argVect[0]（即執行檔）是否為NULL保證main function不是在處理字串*/
        /*主程式的控制迴圈必須在一開始的地方將argVect[0]設為NULL*/
        if (argVect[0] == NULL) {
            ungetc('\n', stdin);ungetc('c', stdin);ungetc('^', stdin);
            siglongjmp(jumpBuf, 1);
        } else {
            /*極少發生，剛好在處理字串，忽略這個signal，印出訊息提示一下*/
            fprintf(stderr, "info, 處理字串時使用者按下ctr-c\n");
        }
    }
}

/*
與myShell.c不一樣的地方，額外實現了dir這個指令
dir可以列出當前目錄裡的所有檔案，功能類似於 ls
*/

int dir()
{
    DIR *dir;
    struct dirent *ent;
    char *curDir = "./";
    char pathname[512];
    struct stat buf;
    int perm;
    char *time;
    
    dir = opendir("./");
    ent = readdir(dir);
    while (ent != NULL)
    {
        strcpy(pathname, curDir);
        strcat(pathname, ent->d_name);
        stat(pathname, &buf);
        perm = (buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
        time = ctime(&buf.st_atime);
        time[strlen(time) - 1] = 0;
        printf("%o  %d  %d %8d %s %s\n", perm, buf.st_uid,
               buf.st_gid, (int)buf.st_size, time, ent->d_name);
        ent = readdir(dir);
    }
    closedir(dir);
    return 0;
}

/*
 parseString：將使用者傳進的命令轉換成字串陣列
 str：使用者傳進的命令
 cmd：回傳執行檔
 */
void parseString(char* str, char** cmd) {
    int idx=0;
    char* retPtr;
    //printf("%s\n", str);
    //這裡使用了strrtok，這個函數可以將字串切割成小字串
    //第二個參數決定「切割的依據」，在此為換行符號「\n」與空白符號「 」
    retPtr=strtok(str, " \n");
    while(retPtr != NULL) {
        //printf("token =%s\n", retPtr);
        //if(strlen(retPtr)==0) continue;
        argVect[idx++] = retPtr;
        if (idx==1) {
            *cmd = retPtr;
            //printf("para = %s\n", *cmd);
        }
        printf("para = %s\n", retPtr);
        retPtr=strtok(NULL, " \n");
    }
    //最後一個字串指標設定為NULL，這樣才知道「已經到字串陣列的結尾」
    argVect[idx]=NULL;
}

int main (int argc, char** argv) {
    char cmdLine[4096];
    char hostName[256];
    char cwd[256];
    char* exeName;
    int pid, pos, wstatus;
    struct rusage resUsage;     //資源使用率，shell可以「wait3()」得到child的資源使用率
    struct timespec statTime, endTime;  //外部程式的開始執行時間、結束時間
    /*底下程式碼註冊signal的處理方式*/
    signal(SIGINT, ctrC_handler);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    
    /*無窮迴圈直到使用者輸入exit*/
    //平常的工作是讀取使用者輸入的字串，然後執行該字串
    while(1) {
        char* showPath;
        char* loginName;
        int homeLen = 0;
        //設定化hasChild, argVect[0]，避免發生race condtion
        hasChild = 0;
        argVect[0]=NULL;
        //抓取主機名稱、用戶名稱，這二個函數是google得到的
        loginName = getlogin();
        gethostname(hostName, 256);
        /*
         底下程式碼製造要顯示的路徑字串，
         會參考"home"將"home"路徑取代為~
         */
        getcwd(cwd, 256);
        //得到當前的工作目錄和家目錄的關係
        //目的是：把/home/shiwulo/Desktop 轉換成 ~/Desktop
        pos=strspn(getenv("HOME"), cwd);
        homeLen = strlen(getenv("HOME"));
        //printf("cwd=%s, home=%s, pos=%d, prompt=%s", cwd, getenv("HOME"), pos, &cwd[pos]);
        if(pos>=homeLen) {
            cwd[pos-1]='~';
            showPath=&cwd[pos-1];
        }
        else
            showPath=cwd;
        /*
         底下程式碼負責印出提示符號
         */
        printf(LIGHT_GREEN"%s@%s:", loginName, hostName);
        printf(BLU_BOLD"%s>> " NONE, showPath);
        //設定返回地點，如果使用者按下ctr-c會從sigsetjmp的下一行開始執行
        //相當於設定書籤
        sigsetjmp(jumpBuf, 1);
        /*
         接收使用者命令，除了cd, exit, dir, ^c 以外，其他指令呼叫對應的執行檔
         */
        fgets(cmdLine, 4096, stdin);
        printf("cmd = %s\n", cmdLine);
        //printf("cmdLine = %s\n",cmdLine);
        if (strlen(cmdLine)>1)  //判斷長度是否大於1，判斷「使用者無聊按下enter鍵」
            parseString(cmdLine, &exeName);
        else
            continue;   //提示：continue是回到「while(1)」的那一行
        //使用者按下control-c，^c是由signal handler「ctrC_handler()」放入
        if (strcmp(exeName, "^c") == 0) {   
            //printf("ctr-c \n");
            printf("\n");
            continue;
        }
        //輸入dir，列出該目錄的所有物件
        if (strcmp(exeName, "dir")==0) {
            dir();
            printf("\n");
            continue;
        }
        //結束shell，因此跳出 while(1)
        if (strcmp(exeName, "exit") == 0)   //內建指令exit
            break;
        //cd指令一定是內建指令，是改變shell的工作目錄，shell的child會繼承這個工作目錄
        if (strcmp(exeName, "cd") == 0) {   //內建指令cd
            if (strcmp(argVect[1], "~")==0)
                chdir(getenv("HOME"));
            else
                chdir(argVect[1]);
            continue;
        }
        //開始計算執行一個「外部指令」花多少時間
        clock_gettime(CLOCK_MONOTONIC, &statTime);
        pid = fork();   //除了exit, cd，其餘為外部指令
        if (pid == 0) {
            //在這個地方如果要避免錯誤，可以呼叫access()確認真的有該執行檔案
            //https://tw.gitbook.net/unix_system_calls/access.html
            if (execve(exeName, argVect, NULL)==-1) {
                perror("myShell");
                exit(errno*-1);
            }
        } else {
            /*
             parent(myShell)等待child完成命令
             完成命令後，parent將child的結束回傳值印出來
             */
            childPid = pid;/*通知singal handler，如果使用者按下ctr-c時，要處理這個child*/
            hasChild = 1;/*通知singal handler，正在處理child*/
            wait3(&wstatus, 0, &resUsage);
            clock_gettime(CLOCK_MONOTONIC, &endTime);
            //wait(&wstatus);
            //int ret=getrusage(RUSAGE_CHILDREN, &resUsage);
            //printf("ret = %d\n", ret);
            double sysTime = timeval2sec(resUsage.ru_stime);
            double usrTime = timeval2sec(resUsage.ru_utime);
            //printf("%ld\n", endTime.tv_nsec);
            //printf("%ld\n", statTime.tv_nsec);
            printf("\n");
            long long elapse = timespec2sec(endTime)-timespec2sec(statTime);
            printf(RED"經過時間:                       "YELLOW"%lld.%llds\n",elapse/nspersec, elapse%nspersec);
            printf(RED"CPU花在執行程式的時間:             "YELLOW"%fs\n"
                   RED"CPU於usr mode執行此程式所花的時間："YELLOW"%fs\n"
                   RED"CPU於krl mode執行此程式所花的時間："YELLOW"%fs\n", sysTime+usrTime , usrTime, sysTime);
            printf(RED"Page fault，但沒有造成I/O：      "YELLOW"%ld\n", resUsage.ru_minflt);
            printf(RED"Page fault，並且觸發I/O:        "YELLOW"%ld\n", resUsage.ru_majflt);
            printf(RED"自願性的context switch：        "YELLOW"%ld\n", resUsage.ru_nvcsw);
            printf(RED"非自願性的context switch:       "YELLOW"%ld\n", resUsage.ru_nivcsw);
            printf(RED "return value of " YELLOW "%s" RED " is " YELLOW "%d\n",
                   exeName, WEXITSTATUS(wstatus));
            //printf("isSignaled? %d\n", WIFSIGNALED(wstatus));
            if (WIFSIGNALED(wstatus))
                printf(RED"the child process was terminated by a signal "YELLOW"%d"RED
                       ", named " YELLOW "%s.\n",  WTERMSIG(wstatus), strsignal(WTERMSIG(wstatus)));
            printf(NONE);
        }
    }
}
