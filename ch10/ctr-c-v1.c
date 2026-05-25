#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

time_t last_ctrlc = 0;
int ctrlc_count = 0;

void sigint_handler(int signo) {
    time_t now = time(NULL);

    if (ctrlc_count == 0 || difftime(now, last_ctrlc) > 2) {
        printf("Press Ctrl-C twice\n");
        ctrlc_count = 1;
        last_ctrlc = now;
    } else {
        exit(0);
    }
}

int main(void) {
    signal(SIGINT, sigint_handler);

    while (1) {
        printf("%ld\n", time(NULL));
        sleep(1);
    }

    return 0;
}
