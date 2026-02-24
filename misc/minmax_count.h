
struct MinMaxCount {
    ll minval, maxval;
    ll mincnt, maxcnt;
};

template<class T>
MinMaxCount countMinMax(const vector<T>& a) {
    const size_t n = a.size();
    if (n == 0) return {T{}, T{}, 0, 0};
    T mn = a[0], mx = a[0];
    ll cmn = 1, cmx = 1;
    for (size_t i = 1; i < n; i++) {
        const T x = a[i];
        if (x < mn) { mn = x; cmn = 1; }
        else if (x == mn) { cmn++; }
        if (x > mx) { mx = x; cmx = 1; }
        else if (x == mx) { cmx++; }
    }
    return {(ll)mn, (ll)mx, cmn, cmx};
}
