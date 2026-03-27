
inline ll binpow(ll a, ll b) {
    ll res = 1;
    for (; b; b>>=1) {
        if (b&1) res = res * a;
        a = a * a;
    }
    return res;
}
