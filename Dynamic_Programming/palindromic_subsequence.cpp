#include "../header.h"

// Longest Palindromic Subsequence. O(n^2)
int LPS(const string& s) {
    int n = sz(s);
    V<vi> dp(n, vi(n, 0));

    // all single characters are palindrome of length 1 by default
    for (int i = 0; i < n; i++) {
        dp[i][i] = 1;
    }

    for (int len = 2; len <= n; len++) {
        for (int l = 0; l <= n-len; l++) {
            int r = l + len - 1;

            if (s[l] == s[r]) {
                dp[l][r] = 2 + (l + 1 <= r - 1 ? dp[l+1][r-1] : 0);
            } else {
                dp[l][r] = max(dp[l+1][r], dp[l][r-1]);
            }
        }
    }

    return dp[0][n-1];
}


// Count All (including duplicates) Palindromic Subsequences. O(n^2)
int count_PS(const string& s) {
    int n = sz(s);
    V<vi> dp(n, vi(n, 0));
    for (int i = 0; i < n; i++) dp[i][i] = 1;

    for (int len = 2; len <= n; len++) {
        for (int l = 0; l <= n-len; l++) {
            int r = l + len - 1;

            int mid = (l + 1 <= r - 1 ? dp[l+1][r-1] : 0);
            if (s[l] == s[r]) {
                dp[l][r] = dp[l+1][r] + dp[l][r-1] + 1;
            } else {
                dp[l][r] = dp[l+1][r] + dp[l][r-1] - mid; // inclusion-exclusion
            }
        }
    }

    return dp[0][n-1];
}

// Count DISTINCT Palindromic Subsequences. O(n^2) + O(n) preprocessing
int count_distinct_PS(const string& s) {
    int n = s.size();
    static constexpr long long MOD = 1e9 + 7;
    vector<vector<long long>> dp(n, vector<long long>(n, 0));
    for (int i = 0; i < n; i++) dp[i][i] = 1;

    vector<int> prev(n, -1), next(n, n);
    unordered_map<char, int> last;

    for (int i = 0; i < n; i++) {
        if (last.count(s[i])) prev[i] = last[s[i]];
        last[s[i]] = i;
    }
    last.clear();

    for (int i = n-1; i >= 0; i--) {
        if (last.count(s[i])) next[i] = last[s[i]];
        last[s[i]] = i;
    }

    for (int len = 2; len <= n; len++) {
        for (int l = 0; l <= n-len; l++) {
            int r = l + len - 1;

            if (s[l] != s[r]) {
                dp[l][r] = (dp[l+1][r] + dp[l][r-1] - dp[l+1][r-1]) % MOD;
                if (dp[l][r] < 0) dp[l][r] += MOD;
            }
            else {
                int lo = next[l];
                int hi = prev[r];
                long long mid = (l + 1 <= r - 1 ? dp[l+1][r-1] : 0);
                if (lo > hi) {
                    // no same char inside [l+1, r-1]
                    dp[l][r] = (2 * mid + 2) % MOD;
                } else if (lo == hi) {
                    // one same char inside
                    dp[l][r] = (2 * mid + 1) % MOD;
                } else {
                    // more than one same char inside: need to subtract palindromes including them
                    dp[l][r] = (2 * mid - dp[lo+1][hi-1]) % MOD;
                    if (dp[l][r] < 0) dp[l][r] += MOD;
                }
            }
        }
    }

    return dp[0][n-1] % MOD;
}

