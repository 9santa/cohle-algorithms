#include <bits/stdc++.h>
#include <ctime>

#define pb push_back
#define eb emplace_back
#define all(a) (a).begin(), (a).end()

using namespace std;

using i64 = long long;
using u64 = unsigned long long;
using u32 = unsigned int;
using u128 = unsigned __int128;
using i128 = __int128;
using ld = long double;
using pii = pair<int, int>;


constexpr int MOD = 1e9+7;

// BINARY EXPONENTIATION
class Exponentiation {
    long long binpowRecursive(long long a, long long power) {
        if (power == 0) return 1;   // a^0 = 1

        long long res = binpowRecursive(a, power / 2); // this will round down if power is odd
        res = (res * res) % MOD;
        if (power&1) res = (res * a) % MOD; // if power is odd, need to multiply one more time

        return res;
    }

    long long binpow(long long a, long long power) {
        long long res = 1;

        while (power > 0) {
            if (power & 1) {
                res = res * a;
            }
            a = a * a;
            power >>= 1;   // div 2
        }
        return res;
    }

    long long binpowMod(long long a, long long power, long long mod) {
        long long res = 1;

        while (power > 0) {
            if (power & 1) {
                res = (res * a) % mod;
            }
            a = (a * a) % mod;
            power >>= 1;    // div 2
        }
        return res;
    }
};

