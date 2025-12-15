#include <bits/stdc++.h>

using namespace std;
using i64 = long long;

i64 bgcd(i64 a, i64 b) {
    if (!a || !b) return a | b; // if one is 0, return the other
    // count how many times (a | b) is divisible by 2
    unsigned shift = __builtin_ctz(a | b);
    a >>= __builtin_ctz(a); // remove factors of 2 from a
    do {
        // remove factors of 2 from b
        b >>= __builtin_ctz(b);
        // make sure a <= b for convenince
        if (a > b) std::swap(a, b);
        b -= a;
    } while (b);
    return a << shift;
}

