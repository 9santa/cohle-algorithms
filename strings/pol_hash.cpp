#include <bits/stdc++.h>

using namespace std;

const int MOD = 998244353;
const double PI = 3.141592653589793;
bool flag = 0;


// идея простая, мы хотим вместо строки хранить число, и желательно не очень большое
// the idea is simple, we want to store a number (preferably not a big one) instead of a string
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

    cout << "Polynomial hash: " << h << endl;
}
