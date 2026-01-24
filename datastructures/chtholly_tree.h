#include "../header.h"

/* Lazy, interval-based ds that uses map
   to store contiguous intervals with the same value
   Efficient for certain types of randomized array ops */

template<typename T>
struct Chtholly {
private:
    map<int, T> mp; // stores intervals [l, r) with value
    int n;

    static int binpow(int a, int b, int mod) {
        int res = 1 % mod;
        for (; b; b >>= 1, a = 1LL * a * a % mod) {
            if (b&1) res = 1LL * res * a % mod;
        }
        return res;
    }

public:
    // Init from array
    Chtholly(const vector<T>& arr) : n((int)arr.size()) {
        for (int i = 0; i < n;) {
            int j = i;
            while (j < n && arr[j] == arr[i]) j++;
            mp[i] = arr[i];
            i = j;
        }
        mp[n] = T(); // sentinel
    }

    // Init with all 'initial'
    Chtholly(int size, T initial = 0) : n(size) {
        mp[0] = initial;
        mp[n] = -1;
    }

private:
    // Split interval at position i
    void split(int i) {
        if (i >= n) return;
        auto it = prev(mp.upper_bound(i));
        if (it->first != i) {
            mp[i] = it->second;
        }
    }

public:
    // Range add: add x to [l, r)
    void range_add(int l, int r, T x) {
        if (l >= r) return;
        split(l);
        split(r);
        for (auto it = mp.find(l); it->first != r; it++) {
            it->second += x;
        }
    }

    void range_assign(int l, int r, T x) {
        if (l >= r) return;
        split(l);
        split(r);
        // Remove intervals within [l, r)
        auto it = mp.find(l);
        while (it->first != r) it = mp.erase(it);
        mp[l] = x;
    }

    // Get k-th smallest element in [l, r) (0-idx)
    T range_kth(int l, int r, int k) {
        if (l >= r || k < 0) return T();
        split(l);
        split(r);

        vector<pair<T, i64>> segments; // {value, length}
        for (auto it = mp.find(l); it->first != r; it++) {
            int len = next(it)->first - it->first;
            segments.emplace_back(it->second, len);
        }

        sort(segments.begin(), segments.end());
        for (auto [val, len] : segments) {
            if (k < len) return val;
            k -= len;
        }

        return T(); // k out of range
    }

    // Get sum of (a[i]^x mod M) over [l, r)
    i64 range_power_sum(int l, int r, int x, int M) {
        if (l >= r) return 0;
        split(l);
        split(r);

        i64 ans = 0;
        for (auto it = mp.find(l); it->first != r; it++) {
            int len = next(it)->first - it->first;
            T val = it->second % M;
            ans = (ans + 1LL * len * binpow(val, x, M)) % M;
        }
        return ans;
    }

    // Get all values in [l, r) as vector of pairs {value, length}
    vector<pair<T, i64>> get_range(int l, int r) {
        if (l >= r) return 0;
        split(l);
        split(r);

        vector<pair<T, i64>> res;
        for (auto it = mp.find(l); it->first != r; it++) {
            int len = next(it)->first - it->first;
            res.emplace_back(it->second, len);
        }
        return res;
    }

    // Get value at position i
    T get(int i) {
        if (i < 0 || i >= n) return T();
        auto it = prev(mp.upper_bound(i));
        return it->second;
    }
};

