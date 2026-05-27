#include "core.h"
// идея простая, мы хотим вместо строки хранить число, и желательно не очень большое
// the idea is simple, we want to store a number (preferably not a big one) instead of a string
/** Demonstrates a simple polynomial hash computation. Time: O(n). Space: O(1). */
void pol_hash()
{
    const int k = 31;
    const int mod = 1000000007; // 1e9+7
    string s = "abacabadaba";

    long long h = 0;
    long long power = 1;

    for (char c : s) {
        int x = (int)(c - 'a' + 1);
        h = (h + x * power) % mod;
        power = (power * k) % mod;
    }
}
