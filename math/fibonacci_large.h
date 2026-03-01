#pragma once
#include "large_number.h"

inline large_number fib(uint64_t n) {
    large_number f0(1);
    large_number f1(1);

    do {
        large_number f2 = f1;
        f1 += f0;
        f0 = f2;
        n--;
    } while (n > 2);

    return f1;
}


