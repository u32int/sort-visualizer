#define _POSIX_C_SOURCE 200809L
#include <time.h>

#include "util.h"

int clamp_int(int a, int min, int max)
{
    if (a > max) return max;
    return a < min ? min : a;
}

void sleep_ms(size_t milis)
{
    struct timespec ts = {
        .tv_nsec = (milis % 1000) * 1000000,
        .tv_sec = milis / 1000
    };

    nanosleep(&ts, NULL);
}
