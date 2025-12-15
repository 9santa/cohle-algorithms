#include <bits/stdc++.h>
#include "prime_factorization.h"

// GCD and LCM are assosiative, so we can do: gcd(a,b,c,d) = gcd(a, gcd(b, gcd(c, d)))
int gcd(int a, int b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

int lcm(int a, int b) {
    return a / gcd(a, b) * b; // (a*b) / gcd(a, b);
}

// function to check if 2 numbers in the array have gcd(a[i], a[j]) > 1
bool gcd_over_one(const vi& a) {
    auto divi = pfac[x];
    for (auto p : divi) {
        if (cnt[p] > 0) {   // cnt - array or map to count divisors
            return true;
        }
        cnt[p]++;
    }
    return false;
}
