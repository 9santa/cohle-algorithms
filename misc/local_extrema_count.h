
// pair.first = local mins, pair.second = local maxs
template<class T>
pair<ll, ll> countLocalExtrema(const vector<T>& a) {
    pair<ll, ll> res;
    const size_t n = a.size();
    if (n < 3) return res;
    // trend: -1 = down, +1 = up, 0 = undefined
    int trend = 0;
    for (size_t i = 1; i < n; i++) {
        if (a[i] == a[i-1]) continue;
        const int dir = (a[i] > a[i-1]) ? +1 : -1;
        if (trend == 0) trend = dir;
        else if (dir != trend) {
            // trend change, extrema found
            if (trend > 0 && dir < 0) ++res.second; // up -> down
            else if (trend < 0 && dir > 0) ++res.first; // down -> up
            trend = dir;
        }
    }
    return res;
}
