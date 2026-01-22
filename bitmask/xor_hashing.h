#include "../header.h"

static u64 splitmix64(u64 x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

// Collision probability: 2^(-64)
template<class T>
static vector<u64> xor_hash(vector<T>& a) {
    int n = sz(a);
    int mx = *max_element(all(a));
    // if we only need permutations of 1..len, values can never be > n
    // but this just to be safe
    int M = max(mx, n);

    // Random value per integer x (1..M)
    // Use a fixed seed (or time-based) depending on preference
    // Fixed seed is fine
    u64 seed = chrono::steady_clock::now().time_since_epoch().count();
    vector<u64> rv(M+1, 0);
    for (int x = 1; x <= M; x++) rv[x] = splitmix64(seed + x);

    return rv;
}

// Queries: is a[l..r] a permutation of if its own value range [L..R]?
static void solve() {
    int n, q; re(n, q);
    vector<int> a(n+1);
    int M = 0;
    for (int i = 1; i <= n; i++) { cin >> a[i]; M = max(M, a[i]); }

    auto rv = xor_hash(a);

    // Prefix XOR hash of array: pref[i] = rv[a1] xor ... xor rv[ai]
    vector<u64> prefPos(n+1, 0);
    for (int i = 1; i <= n; i++) {
        u64 h = 0;
        if (a[i] >= 1 && a[i] <= M) h = rv[a[i]];
        prefPos[i] = prefPos[i-1] ^ h;
    }

    vector<u64> prefVal(M+1, 0);
    for (int i = 1; i <= M; i++) {
        prefVal[i] = prefVal[i-1] ^ rv[i];
    }

    // Prefix XOR hash of target permutation:
    // permX[len] = rv[1] xor rv[2] xor ... xor rv[len]
    vector<u64> permX(n+1, 0);
    for (int len = 1; len <= n; len++) permX[len] = permX[len-1] ^ rv[len];

    // Sparse table for range min/max in O(1)
    vector<int> lg(n+1, 0);
    for (int i = 2; i <= n; i++) lg[i] = lg[i/2] + 1;
    int K = lg[n];

    V<vi> stmin(K+1, vi(n+1));
    V<vi> stmax(K+1, vi(n+1));
    for (int i = 1; i <= n; i++) stmin[0][i] = stmax[0][i] = a[i];
    for (int k = 1; k <= K; k++) {
        for (int i = 1; i + (1<<k) - 1 <= n; i++) {
            stmin[k][i] = min(stmin[k-1][i], stmin[k-1][i + (1 << (k-1))]);
            stmax[k][i] = max(stmax[k-1][i], stmax[k-1][i + (1 << (k-1))]);
        }
    }

    auto rangeMin = [&](int l, int r) {
        int k = lg[r-l+1];
        return min(stmin[k][l], stmin[k][r - (1 << k) + 1]);
    };
    auto rangeMax = [&](int l, int r) {
        int k = lg[r-l+1];
        return max(stmax[k][l], stmax[k][r - (1 << k) + 1]);
    };

    // Answer queries
    while (q--) {
        int l, r; re(l, r);
        int len = r - l + 1;

        int mini = rangeMin(l, r);
        int maxi = rangeMax(l, r);
        // Neccessary condition for a permutation of 1..len
        // if (mini != 1 || maxi != len) {
        //     cout << "NO\n";
        //     continue;
        // }

        if (maxi - mini + 1 != len) {
            cout << "NO\n";
            continue;
        }

        u64 subHash = prefPos[r] ^ prefPos[l-1];
        u64 target = prefVal[maxi] ^ prefVal[mini-1];

        // if (subHash == permX[len]) cout << "YES\n";
        cout << (subHash == target ? "YES\n" : "NO\n");
    }
}
