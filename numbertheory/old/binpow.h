
inline ll binpow(ll a, ll b, ll MOD) {
    ll res = 1;
    for (a %= MOD; b; b>>=1) {
        if (b&1) res = (res * a) % MOD;
        a = (a * a) % MOD;
    }
    return res;
}
