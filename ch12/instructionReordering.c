#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h> 
#include <stdatomic.h>
#include <sys/syscall.h>
#include <signal.h>
#include <assert.h>

volatile atomic_int a, b, c, d, e, f;

int main(int argc, char** argv) {
    int w, x, y, z, l , m, n;
    atomic_int vol_local;
    w = 0x11;
    //x = 0x22222;
    //y = 0x33333;
    //z = 0x44444;
    x = &w;
    x = &w + 0x22;
    atomic_fetch_add_explicit(&vol_local, 1, memory_order_relaxed);

    a=0xaa;
    //b=0xbbbb|random();
    //c=0xdddd|random();
    //e=0xeeee|random();
    //f=0xffff|random();
    //printf("%d\n", a|b|c|d|e|f|w|x|y|z);
    printf("%d\n", a|w|x);
}