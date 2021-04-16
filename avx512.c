#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <immintrin.h>
#include <stdatomic.h>

long ts2ns(struct timespec ts)
{
        return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

long long cache[2048*2048+1];

int main()
{
        __m512i temp1[32];       //64*8=2K
        __m512i temp2[3200];
        __m512i zero={0};
        atomic_llong *ary1 = (atomic_llong *)temp1;
        atomic_llong *ary2 = (atomic_llong *)&temp2[1024];

        //printf("sizeof atomic_llong is %ld\n", sizeof(atomic_llong));
        for (int i=0; i< 256; i++) {
                //ary1[i]=i;    //驗證用
        }

        //清空cache
        for (int i=0; i< 2048*2048; i++) {
                cache[i] = cache[i+1];
        }
        //避免編譯器優化
        printf("避免編譯器優化的輸出 %lld\n", cache[2048 * 2048]);

        for (int i = 0; i < 256; i++)
        {
                //printf("%lld, ",ary1[i]);     //驗證用
        }

        //printf("\n\n\n");
        __asm__(
            "vpxorq %%zmm0, %%zmm0, %%zmm0;\n"
            :           //output, b的代號是"%0"
            :           //input, a代號是"%1", da代號是"%2"
            :           //搞髒掉的暫存器，gcc會幫我們還原
        );

        struct timespec time1, time2;
        clock_gettime(CLOCK_MONOTONIC, &time1);
        //2048 byte
        __asm__(
            //"VMOVDQA64 (%1), %%zmm0\n"
            "vmovdqa64 %%zmm0, 0x00(%0)\n"
            "vmovdqa64 %%zmm0, 0x40(%0)\n"
            "vmovdqa64 %%zmm0, 0x80(%0)\n"
            "vmovdqa64 %%zmm0, 0x0c0(%0)\n"
            "vmovdqa64 %%zmm0, 0x100(%0)\n"
            "vmovdqa64 %%zmm0, 0x140(%0)\n"
            "vmovdqa64 %%zmm0, 0x180(%0)\n"
            "vmovdqa64 %%zmm0, 0x1C0(%0)\n"
            "vmovdqa64 %%zmm0, 0x200(%0)\n"
            "vmovdqa64 %%zmm0, 0x240(%0)\n"
            "vmovdqa64 %%zmm0, 0x280(%0)\n"
            "vmovdqa64 %%zmm0, 0x2C0(%0)\n"
            "vmovdqa64 %%zmm0, 0x300(%0)\n"
            "vmovdqa64 %%zmm0, 0x340(%0)\n"
            "vmovdqa64 %%zmm0, 0x380(%0)\n"
            "vmovdqa64 %%zmm0, 0x3C0(%0)\n"
            "vmovdqa64 %%zmm0, 0x400(%0)\n"
            "vmovdqa64 %%zmm0, 0x440(%0)\n"
            "vmovdqa64 %%zmm0, 0x480(%0)\n"
            "vmovdqa64 %%zmm0, 0x4C0(%0)\n"
            "vmovdqa64 %%zmm0, 0x500(%0)\n"
            "vmovdqa64 %%zmm0, 0x540(%0)\n"
            "vmovdqa64 %%zmm0, 0x580(%0)\n"
            "vmovdqa64 %%zmm0, 0x5C0(%0)\n"
            "vmovdqa64 %%zmm0, 0x600(%0)\n"
            "vmovdqa64 %%zmm0, 0x640(%0)\n"
            "vmovdqa64 %%zmm0, 0x680(%0)\n"
            "vmovdqa64 %%zmm0, 0x6C0(%0)\n"
            "vmovdqa64 %%zmm0, 0x700(%0)\n"
            "vmovdqa64 %%zmm0, 0x740(%0)\n"
            "vmovdqa64 %%zmm0, 0x780(%0)\n"
            "vmovdqa64 %%zmm0, 0x7C0(%0)\n"
            :            //output
            : "g"(temp1), "g"(zero) //input
            :            //搞髒掉的暫存器，gcc會幫我們還原
        );
        clock_gettime(CLOCK_MONOTONIC, &time2);
        printf("AVX花在prefetch的時間 = %ld ns\n", ts2ns(time2) - ts2ns(time1));
        /*
        for (int i = 0; i < 32 * 8; i++)
        {
                printf("%lld, ", ary1[i]); //驗證用
        }
        printf("\n\n\n");
        */
        for (int i=0; i<32; i++) {
                __asm__(
                "vmovdqa64 %%zmm0, 0x0(%0)\n"
                :            //output, b的代號是"%0"
                : "g"(temp1+i) //input, a代號是"%1", da代號是"%2"
                :            //搞髒掉的暫存器，gcc會幫我們還原
                );
        }
        /*
        for (int i=0; i<32*8; i++) {
                printf("%lld, ", ary1[i]); //驗證用
        }
        */
        printf("AVX花在prefetch的時間（沒展開） = %ld ns\n", ts2ns(time2) - ts2ns(time1));

        clock_gettime(CLOCK_MONOTONIC, &time1);
        for (int i = 0; i < 32; i++)
        {
                atomic_load_explicit(ary1 + i * 8, memory_order_relaxed);
        }
        clock_gettime(CLOCK_MONOTONIC, &time2);
        printf("存取被AVX prefetch過的陣列 = %ld ns\n", ts2ns(time2) - ts2ns(time1));


        clock_gettime(CLOCK_MONOTONIC, &time1);
        for (int i = 0; i < 32; i++)
        {
                atomic_load_explicit(ary2 + i*8, memory_order_relaxed);
        }
        clock_gettime(CLOCK_MONOTONIC, &time2);
        printf("使用普通load以prefetch ary2 %ld ns\n", ts2ns(time2) - ts2ns(time1));

        clock_gettime(CLOCK_MONOTONIC, &time1);
        for (int i = 0; i < 32; i++)
        {
                atomic_load_explicit(ary2 + i*8, memory_order_relaxed);
        }
        clock_gettime(CLOCK_MONOTONIC, &time2);
        printf("存取prefetch過的ary2的時間= %ld ns\n", ts2ns(time2) - ts2ns(time1));

        return 0;
}
