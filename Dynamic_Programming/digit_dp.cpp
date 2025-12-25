#include "../header.h"

// Count numbers <= N with predicate: digits are non decreasing
// O(len(N) * 10 * 2 * 10)
ll non_decreasing_digits(ll N) {
    vector<int> digits;
    if (N == 0) digits.push_back(0);
    while (N > 0) {
        digits.push_back(N % 10);
        N /= 10;
    }
    reverse(all(digits));
    int len = digits.size();

    // dp[i][d][flag] - count of satisfying numbers with len i, last digits d, flag = 1 if < N, else flag = 0
    vector<vector<vector<ll>>> dp(len+1, vector<vector<ll>>(10, vector<ll>(2, 0)));
    dp[0][0][0] = 1;

    for (int i = 0; i < len; i++) {
        for (int d = 0; d <= 9; d++) {
            for (int flag = 0; flag <= 1; flag++) {
                ll cur = dp[i][d][flag];
                if (!cur) continue;

                int limit = flag ? 9 : digits[i];

                for (int nd = 0; nd <= limit; nd++) {
                    if (i > 0 && nd < d) continue;

                    int nflag = flag || (nd < limit);
                    dp[i+1][nd][nflag] += cur;
                }
            }
        }
    }

    ll ans = 0;
    for (int d = 0; d <= 9; d++) {
        ans += dp[len][d][0];
        ans += dp[len][d][1];
    }

    return ans;
}
