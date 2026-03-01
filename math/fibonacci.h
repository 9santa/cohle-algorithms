#pragma once
#include "../header.h"

static constexpr u64 FIB_MAX_N_U64 = 93; // F(93) fits in u64, F(94) does not

// 1) O(n) iterative
inline u64 fib_iter(u64 n) {
    u64 a = 0, b = 1;
    for (u64 i = 0; i < n; i++) {
        u64 nxt = a + b;
        a = b;
        b = nxt;
    }
    return a;
}

// 2) O(log n) Lucas squaring
inline u64 fib_lucas(u64 n) {
    if (n < 2) return n;
    if (n <= 10) return fib_iter(n); // for small n simple loop is good

    u64 i = n;
    u64 a = i & 1ULL;
    i64 b = (i & 1ULL) ? -1LL : 1LL;
    i64 c = 3;

    i >>= 1;
    while (i >= 2) {
        if ((i & 1ULL) == 0) b = (i64)a + b * c;
        else a = (u64)((i64)b + (i64)a * c);
        c = c * c - 2;
        i >>= 1;
    }
    // Final combine
    i64 res = b + (i64)a * c;
    return (u64)res;
}

// 3) O(log n) fast doubling
// Returns (F(n), F(n+1))
inline pair<u64, u64> fib_doubling_pair(u64 n) {
    if (n == 0) return {0, 1};
    auto [a, b] = fib_doubling_pair(n >> 1); // a=F(k), b = F(k+1)
    // c = F(2k)    = F(k) * (2*F(k+1) - F(k))
    // d = F(2k+1)  = F(k)^2 + F(k+1)^2
    u128 two_b_minus_a = (u128)2 * b - a;
    u128 c = (u128)a * two_b_minus_a;
    u128 d = (u128)a * a + (u128)b * b;

    if (n & 1ULL) return {(u64)d, (u64)(c+d)};
    return {(u64)c, (u64)d};
}

// 4) O(log n) matrix exponentiation
// M = [[1, 1], [1, 0]]; M^n = [[F(n+1), F(n)], [F(n), F(n-1)]]
struct Mat2 {
    u64 a, b, c, d; // [[a,b], [c,d]]
};

inline Mat2 mul(const Mat2& x, const Mat2& y) {
    u128 A = (u128)x.a * y.a + (u128)x.b * y.c;
    u128 B = (u128)x.a * y.b + (u128)x.b * y.d;
    u128 C = (u128)x.c * y.a + (u128)x.d * y.c;
    u128 D = (u128)x.c * y.b + (u128)x.d * y.d;
    return {(u64)A, (u64)B, (u64)C, (u64)D};
}

inline u64 fib_matrix(u64 n) {
    if (n == 0) return 0;
    Mat2 base{1, 1, 1, 0};
    Mat2 res{1, 0, 0, 1}; // identity

    u64 e = n;
    while (e) {
        if (e & 1ULL) res = mul(res, base);
        base = mul(base, base);
        e >>= 1;
    }
    return res.b; // F(n)
}

// 5) Floating Binet formula (drifts at some point)
inline u64 fib_binet(u64 n) {
    long double sqrt5 = sqrtl(5.0L);
    long double golden_ratio = (1.0L + sqrt5) / 2.0L;
    long double inv_golden_ratio = -1.0L / golden_ratio;
    long double res = powl(golden_ratio, (long double)n) - powl(inv_golden_ratio, (long double)n) / sqrt5;
    return (u64) (llroundl(res));
}
