#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main(void) {
    while (1) {
        printf("Running... %ld\n", time(NULL));
        sleep(1);
    }

    return 0;
}
