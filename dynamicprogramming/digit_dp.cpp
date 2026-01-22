#include "../header.h"

// Count numbers <= N with predicate: digits are non decreasing
// For segment [L, R] ans = solve(R) - solve(L-1)
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

// Count numbers with digit sum = S
// Iterative bottom-up
ll count_numbers_with_digit_sum_iterative(ll N, ll S) {
    vector<int> digits;
    if (N == 0) digits.push_back(0);
    while (N > 0) {
        digits.push_back(N % 10);
        N /= 10;
    }
    reverse(all(digits));
    int len = sz(digits);

    vector<vector<vector<ll>>> dp(len+1, vector<vector<ll>>(2, vector<ll>(200, 0)));
    dp[0][1][0] = 1;

    for (int i = 0; i < len; i++) {
        for (int tight = 0; tight <= 1; tight++) {
            for (int sum = 0; sum <= S; sum++) {
                ll cur = dp[i][tight][sum];
                if (!cur) continue;

                int limit = tight ? digits[i] : 9;
                for (int nd = 0; nd <= limit; nd++) {
                    int next_sum = sum + nd;
                    if (next_sum > S) continue;

                    int next_tight = tight && (nd == limit);
                    dp[i+1][next_tight][next_sum] += cur;
                }
            }
        }
    }

    return dp[len][0][S] + dp[len][1][S];
}

// Recursive DFS
ll dp[20][2][200];
ll count_numbers_with_digit_sum_dfs(int pos, bool tight, int sum, vector<int>& digits, ll S) {
    // base case
    if (pos == digits.size())
        return (sum == S);

    // memo
    if (dp[pos][tight][sum] != -1)
        return dp[pos][tight][sum];

    ll ans = 0;
    int limit = tight ? digits[pos] : 9;

    for (int dig = 0; dig <= limit; dig++) {
        ans += count_numbers_with_digit_sum_dfs(pos+1, tight && (dig == limit), sum + dig, digits, S);
    }

    return dp[pos][tight][sum] = ans;
}
