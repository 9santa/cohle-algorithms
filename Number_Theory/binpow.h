using ll = long long;

ll binpow(ll a, ll power, ll mod) {
    ll res = 1;
    a %= mod;

    while (power > 0) {
        if (power & 1) {
            res = (res * a) % mod;
        }
        a = (a * a) % mod;
        power >>= 1;   // div 2
    }
    return res;
}
