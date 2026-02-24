#include "fft.h"

vi to_base(const string& s, int B, int k) {
    vi a;
    for (int i = sz(s); i > 0; i -= k) {
        int start = max(0, i - k);
        string st = s.substr(start, i - start);
        a.push_back(stoi(st));
    }
    while (!a.empty() && a.back() == 0) a.pop_back();
    return a;
}

string to_decimal(const vl& a, int chunk, bool neg) {
    string res = (neg ? "-" : "");
    res += to_string(a.back());
    for (int i = sz(a)-2; i >= 0; i--) {
        string part = to_string(a[i]);
        res += string(chunk - sz(part), '0') + part;
    }
    return res;
}

string mul_bigint_fft(string s1, string s2) {
    bool neg = false;
    if (!s1.empty() && s1.front() == '-') { neg = !neg; s1.erase(s1.begin()); }
    if (!s2.empty() && s2.front() == '-') { neg = !neg; s2.erase(s2.begin()); }
    auto is_all_zero = [](const string& s) {
        for (char c : s) if (c != '0') return false;
        return true;
    };
    if (is_all_zero(s1) || is_all_zero(s2)) return "0";

    const int base = 1e4;
    const int k = 4;
    auto a = to_base(s1, base, k);
    auto b = to_base(s2, base, k);

    int need = sz(a) + sz(b) - 1;
    int n = 1;
    while (n < need) n <<= 1;
    FFT cache; cache.init(n);
    auto c = convolution(a, b);

    ll carry = 0;
    for (int i = 0; i < sz(c); i++) {
        ll cur = c[i] + carry;
        c[i] = cur % base;
        carry = cur / base;
    }
    while (carry) {
        c.push_back(carry % base);
        carry /= base;
    }
    while (sz(c) > 1 && c.back() == 0) c.pop_back();

    auto res = to_decimal(c, k, neg);
    return res;
}
