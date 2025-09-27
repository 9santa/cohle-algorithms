#include <bits/stdc++.h>
#include <cmath>
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

// GCD and LCM are assosiative, so we can do: gcd(a,b,c,d) = gcd(a, gcd(b, gcd(c, d)))
int gcd(int a, int b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

int lcm(int a, int b) {
    return (a*b) / gcd(a, b);
}



