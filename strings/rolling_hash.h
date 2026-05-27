#include "core.h"

/* Use cases
- substring equality
- palindrome queries
- count distinct substrings
- binary search on substring length
- pattern matching
- suffix/prefix comparasions */

// Must: 0 < base < mod
/** Single-mod rolling hash for lowercase strings. Space: O(n). */
struct RollingHash {
    static const ll MOD = 1e9 + 7LL;
    static const ll BASE = 911382323LL;
    vector<ll> pref;
    vector<ll> pw;

    /** Builds prefix hashes and powers for s. Time: O(n). */
    RollingHash(const string& s) {
        int n = (int)s.size();
        pref.assign(n+1, 0);
        pw.assign(n+1, 1);
        for (int i = 0; i < n; i++) {
            pw[i+1] = (pw[i] * BASE) % MOD;
            pref[i+1] = (pref[i] * BASE + (s[i] - 'a' + 1)) % MOD;
        }
    }

    // [l, r]
    /** Returns hash of inclusive substring [l, r]. Time: O(1). */
    ll getHash(int l, int r) const {
        ll res = pref[r+1] - (pref[l] * pw[r-l+1]) % MOD;
        if (res < 0) res += MOD;
        return res;
    }

    /** Tests equality of two substrings by hash. Time: O(1). */
    bool equalSubstrings(int l1, int r1, int l2, int r2) const {
        if (r1 - l1 != r2 - l2) return false;
        return getHash(l1, r1) == getHash(l2, r2);
    }

};


// More collision stable
/** Double-mod rolling hash for lower collision probability. Space: O(n). */
struct DoubleRollingHash {
    static const ll MOD1 = 1000000007LL;
    static const ll MOD2 = 1000000009LL;
    static const ll BASE = 911382323LL;

    vector<ll> pref1, pref2, pw1, pw2;

    /** Builds prefix hashes and powers for s. Time: O(n). */
    DoubleRollingHash(const string& s) {
        int n = (int)s.size();
        pref1.assign(n+1, 0);
        pref2.assign(n+1, 0);
        pw1.assign(n+1, 1);
        pw2.assign(n+1, 1);

        for (int i = 0; i < n; i++) {
            int x = s[i] - 'a' + 1;
            pw1[i + 1] = (pw1[i] * BASE) % MOD1;
            pw2[i + 1] = (pw2[i] * BASE) % MOD2;

            pref1[i + 1] = (pref1[i] * BASE + x) % MOD1;
            pref2[i + 1] = (pref2[i] * BASE + x) % MOD2;
        }
    }

    /** Returns double hash of inclusive substring [l, r]. Time: O(1). */
    pair<ll, ll> getHash(int l, int r) const {
        ll a = pref1[r + 1] - pref1[l] * pw1[r-l+1] % MOD1;
        if (a < 0) a += MOD1;

        ll b = pref2[r + 1] - pref2[l] * pw2[r-l+1] % MOD2;
        if (b < 0) b += MOD2;

        return {a, b};
    }
};
