#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

time_t last_ctrlc = 0;
int ctrlc_count = 0;
volatile sig_atomic_t stop = 0;

void sigint_handler(int signo) {
    (void)signo;

    time_t now = time(NULL);

    if (ctrlc_count == 0 ||
        difftime(now, last_ctrlc) > 2) {
        ctrlc_count = 1;
        last_ctrlc = now;
    } else {
        stop = 1;
    }
}

int main(void) {
    signal(SIGINT, sigint_handler);

    while (1) {
        if (stop == 1) break;

        printf("%ld\n", time(NULL));
        sleep(1);
    }

    printf("釋放資源，優雅地結束\n");

    return 0;
}
