#include "../header.h"
#include <algorithm>
#include <numeric>

// Dynamic bitset for W >= 10^8
ll knapsack_dynamic_bitset(const vector<int>& weights, int W) {
    const int BITS = 64;

    class BigBitset {
    private:
        vector<u64> bits;
        size_t n;   // total number of bits

        size_t words() const { return (n + BITS - 1) / BITS; }

    public:
        BigBitset(size_t size) : n(size) {
            bits.assign(words(), 0ULL);
        }

        void setbit(size_t pos) {
            if (pos >= n) return;
            bits[pos / BITS] |= (1ULL << (pos % BITS));
        }

        bool testbit(size_t pos) const {
            if (pos >= n) return false;
            return bits[pos / BITS] & (1ULL << (pos % BITS));
        }

        void operator|=(const BigBitset& other) {
            size_t sz = min(bits.size(), other.bits.size());
            for (int i = 0; i < sz; i++) {
                bits[i] |= other.bits[i];
            }
        }


        BigBitset left_shift(size_t k) const {
            if (k >= n) return BigBitset(n);

            BigBitset res(n);
            size_t word_shift = k / BITS;
            size_t bit_shift = k % BITS;

            for (int i = 0; i < bits.size(); i++) {
                if (i + word_shift >= res.bits.size()) break;

                // shift within the word
                if (bit_shift != 0) {
                    res.bits[i + word_shift] |= (bits[i] << bit_shift);
                    if (i + word_shift + 1 < res.bits.size()) {
                        res.bits[i + word_shift + 1] |= (bits[i] >> (BITS - bit_shift));
                    }
                } else { // need to shift the whole word
                    res.bits[i + word_shift] |= bits[i];
                }
            }

            return res;
        }

        void or_shift(size_t k) {
            if (k == 0 || k >= n) return;

            BigBitset temp = left_shift(k);
            *this |= temp;
        }

        size_t highest_bit_below_or_equal(size_t maxval) const {
            size_t pos = min(maxval, n-1);
            for (; pos != size_t(-1); pos--) {
                if (testbit(pos)) return pos;
            }
            return 0;
        }
    };

    size_t bit_size = W + 1;
    BigBitset bs(bit_size);
    bs.setbit(0);

    for (auto w : weights) {
        if (w > W) continue;
        bs.or_shift(w);
    }

    ll ans = bs.highest_bit_below_or_equal(W);
    return ans;
}

// Bitset knapsack. Checks all the weights we can achieve (subset sum). O(n * W / 64)
void knapsack_bitset(const vector<int>& weights, int S) {
    constexpr int MAXW = 100000;
    bitset<MAXW+1> dp;
    dp[0] = 1;
    // parent[s] stores index of item used to form sum s (fits in int16_t if n <= 32767)
    vector<int16_t> parent(S+1, -1);

    for (int i = 0; i < sz(weights); i++) {
        int w = weights[i];
        if (w > S) continue;

        // for reconstruction, storing the index of an item when the sum 's' was first created by using item 'i'
        auto shifted = dp << w;
        auto newly = shifted & ~dp;
        for (int s = newly._Find_first(); s <= S; s = newly._Find_next(s)) {
            parent[s] = i;
        }

        dp |= shifted;
    }

    // reconstruction
    int cur = S;
    while (cur >= 0 && !dp[cur]) cur--;

    vector<int> picked;
    while (cur > 0) {
        int i = parent[cur];
        picked.pb(weights[i]);
        cur -= weights[i];
    }
}


// The most basic: no costs. Goal: find the maximum weight you can get
int basic_knapsack(const vector<int>& w, int W) {
    int n = sz(w);
    V<vi> dp(n+1, vi(W+1, 0));

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= W; j++) {
            dp[i][j] = dp[i-1][j];
            if (w[i-1] <= j) {
                dp[i][j] = max(dp[i][j], dp[i-1][j - w[i-1]] + w[i-1]);
            }
        }
    }

    return dp[n][W];
}

// Subset sum variant for knapsack problem. Finds all the weights we can achieve
int basic_knapsack1(const vector<int>& w, int W){
    int n = sz(w);
    V<vb> dp(n+1, vector<bool>(W+1, false));
    for (int i = 0; i <= n; i++) dp[i][0] = true;

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= W; j++) {
            dp[i][j] = dp[i-1][j];
            if (j >= w[i-1])
                dp[i][j] = (dp[i-1][j] || dp[i-1][j - w[i-1]]);
        }
    }

    int ans = -1;
    for (int j = W; j >= 0; j--) {
        if (dp[n][j] == true) {
            ans = j;
            break;
        }
    }

    return ans;
}

// 0/1 knapsack. sqrt(W) trick. O(n * sqrt(W) + 2 ^ sz(heavy)). Works when most items are small, only few large
int knapsack_sqrt(vector<int>& weights, vector<int>& costs, int W) {
    vector<int> lightw, lightv, heavyw, heavyv;
    int sqrtW = sqrt(W);

    for (int i = 0; i < sz(weights); i++) {
        if (weights[i] <= sqrtW) lightw.push_back(weights[i]), lightv.push_back(costs[i]);
        else heavyw.push_back(weights[i]), heavyv.push_back(costs[i]);
    }

    vector<int> dp(W+1, 0);
    for (int i = 0; i < sz(lightw); i++) {
        for (int w = W; w >= lightw[i]; w--) {
            dp[w] = max(dp[w], dp[w - lightw[i]] + lightv[i]);
        }
    }

    int ans = 0;
    int L = sz(heavyw);
    for (int mask = 0; mask < (1<<L); mask++) {
        int wsum = 0, vsum = 0;
        for (int i = 0; i < L; i++) {
            if (mask&(1<<i)) { wsum += heavyw[i]; vsum += heavyv[i]; }
        }
        if (wsum <= W) ans = max(ans, vsum + dp[W - wsum]);
    }
    return ans;
}

// 0/1 knapsack. MITM for small n <= 40. O(2^(n/2) * log(2^(n/2)))
int knapsack_mitm(vector<int>& weights, vector<int>& costs, int W) {
    int n = sz(weights);
    int n1 = n/2, n2 = n-n1;
    vector<pair<int, int>> left, right;

    for (int mask = 0; mask < (1 << n1); mask++) {
        int w = 0, v = 0;
        for (int i = 0; i < n1; i++) if (mask&(1<<i)) { w += weights[i]; v += costs[i]; }
        if (w <= W) left.push_back({w, v});
    }

    for (int mask = 0; mask < (1 << n2); mask++) {
        int w = 0, v = 0;
        for (int i = 0; i < n2; i++) if (mask&(1<<i)) { w += weights[i+n1]; v += costs[i+n1]; }
        if (w <= W) right.push_back({w, v});
    }

    sort(all(right));
    vector<pair<int, int>> right_filter;
    int maxV = 0;
    for (auto p : right) {
        maxV = max(maxV, p.second);
        if (right_filter.empty() || right_filter.back().second < maxV)
            right_filter.push_back({p.first, maxV});
    }

    int ans = 0;
    for (auto p : left) {
        int w1 = p.first, v1 = p.second;
        int l = 0, r = sz(right_filter)-1, best = 0;
        while (l <= r) {
            int m = (l+r)/2;
            if (right_filter[m].first + w1 <= W) {
                best = right_filter[m].second;
                l = m+1;
            }
        }
        ans = max(ans, v1+best);
    }
    return ans;
}

// 0/1 knapsack. O(n * W). Costs, weights. Goal: maximize the total cost. Answer reconstruction
int knapsack0_1(const vector<int>& c, const vector<int>& w, int W) {
    int n = sz(w);
    V<vi> dp(n+1, vi(W+1, 0));

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= W; j++) {
            dp[i][j] = dp[i-1][j];
            if (w[i-1] <= j) {
                dp[i][j] = max(dp[i][j], dp[i-1][j - w[i-1]] + c[i-1]);
            }
        }
    }

    // reconstruct solution if needed
    vector<int> chosen;
    int remw = W;

    for (int i = n; i > 0; i--) {
        if (remw <= 0) break;

        // if dp[i][remw] != dp[i-1][remw] -> then item i-1 was taken
        if (dp[i][remw] != dp[i-1][remw]) {
            chosen.pb(i-1);
            remw -= w[i-1];
        }
    }
    reverse(all(chosen));
    // return {dp[n][W], chosen};

    return dp[n][W];
}

// 0/1 knapsack. DP by value. O(n * V). Best when costs are small, W is large
ll knapsack_dp_value(vector<int>& weights, vector<int>& costs, int W) {
    int n = sz(weights);
    ll V = accumulate(all(costs), 0LL);
    vector<ll> dp(V+1, infty<ll>);
    dp[0] = 0;

    for (int i = 0; i < n; i++) {
        for (int v = V; v >= costs[i]; v--) {
            dp[v] = min(dp[v], dp[v - costs[i]] + weights[i]);
        }
    }

    for (int v = V; v >= 0; v--) {
        if (dp[v] <= W) return v;
    }
    return 0;
}

// 0/1 knapsack. O(n * W). Space optimized knapsack. O(W) memory
int knapsack_mem_optimized(const vector<int>& c, const vector<int>& w, int W) {
    int n = sz(w);
    vector<int> dp(W+1, 0);

    for (int i = 0; i < n; i++) {
        for (int j = W; j >= w[i]; j--) {
            dp[j] = max(dp[j], dp[j - w[i]] + c[i]);
        }
    }

    return dp[W];
}

// 0/1 knapsack. O(n * W). 2*W memory. Alternating arrays approach. Keeping only 2 rows.
int knapsack_mem_optimized1(const vector<int>& c, const vector<int>& w, int W) {
    int n = sz(w);
    vector<int> odp(W+1, 0), ndp(W+1, 0);
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= W; j++) {
            ndp[j] = odp[j];
            if (w[i-1] <= j)
                ndp[j] = max(ndp[j], odp[j - w[i-1]] + c[i-1]);
        }
        swap(odp, ndp);
    }
    return odp[W];
}

// 0/1 knapsack. O(n * W). Answer reconstruction
pair<int, vector<int>> knapsack_reconstruction(const vector<int>& c, const vector<int>& w, int W) {
    int n = sz(w);
    vector<int> dp(W+1, 0);
    vector<vector<bool>> took(n, vector<bool>(W+1, false));

    for (int i = 0; i < n; i++) {
        for (int j = W; j >= w[i]; j--) {
            int newval = dp[j - w[i]] + c[i];
            if (newval > dp[j]) {
                dp[j] = newval;
                took[i][j] = true;  // took item i at capacity j
            }
        }
    }

    vector<int> chosen;
    int remw = W;

    for (int i = n-1; i >= 0; i--) {
        if (took[i][remw]) {
            chosen.pb(i);
            remw -= w[i];
        }
    }

    reverse(all(chosen));
    return {dp[W], chosen};
}

// 0/1 knapsack. Naive recursion. O(2^n)
int knapsack01_naive(int n, int W, vector<int>& weights, vector<int>& costs) {
    if (n == 0 || W == 0) return 0;
    if (weights[n-1] > W) return knapsack01_naive(n-1, W, weights, costs);
    return max(knapsack01_naive(n-1, W, weights, costs), // don't take
            costs[n-1] + knapsack01_naive(n-1, W-weights[n-1], weights, costs) // take
        );
}

// --- Unbounded Knapsack ---
// Unbounded knapsack. Naive recursion. O(n ^ W) terrible
int unbounded_naive(int i, int W, vector<int>& weights, vector<int>& costs) {
    if (i == 0 || W == 0) return 0;
    if (weights[i-1] > W) return unbounded_naive(i-1, W, weights, costs);
    return max(unbounded_naive(i-1, W, weights, costs), // don't take
            costs[i-1] + unbounded_naive(i, W - weights[i-1], weights, costs) // take
        );
}

// Unbounded knapsack. O(n * W). Forward iteration for unbounded knapsack
int unbounded_dp(vector<int>& weights, vector<int>& costs, int W) {
    int n = sz(weights);
    vector<int> dp(W+1, 0);
    for (int i = 0; i < n; i++) {
        for (int w = weights[i]; w <= W; w++) {
            dp[w] = max(dp[w], dp[w - weights[i]] + costs[i]);
        }
    }
    return dp[W];
}

// --- Bounded Knapsack ---
// Bounded knapsack. O(n * W * k_max). Can only take up to cnt[i] item i. Naive recursion
int bounded_naive(int i, int W, vector<int>& weights, vector<int>& costs, vector<int>& cnt) {
    if (i == 0 || W == 0) return 0;
    int ans = bounded_naive(i-1, W, weights, costs, cnt);   // don't take
    for (int k = 1; k <= cnt[i-1] && k * weights[i-1] <= W; k++) {
        ans = max(ans, k*costs[i-1] + bounded_naive(i-1, W - k*weights[i-1], weights, costs, cnt));
    }
    return ans;
}

// Bounded knapsack. Binary decomposition DP. O(W * sum(log(Ki)))
int bounded_dp_binary(int W, vector<int>& weights, vector<int>& costs, vector<int>& cnt) {
    int n = sz(weights);
    vector<int> new_wt, new_val;
    for (int i = 0; i < n; i++) {
        int c = cnt[i];
        int pwr = 1;
        while (c > 0) {
            int take = min(pwr, c);
            new_wt.push_back(take * weights[i]);
            new_val.push_back(take * costs[i]);
            c -= take;
            pwr <<= 1;
        }
    }

    vector<int> dp(W+1, 0);
    for (int i = 0; i < sz(new_wt); i++) {
        for (int w = W; w >= new_wt[i]; w--) {
            dp[w] = max(dp[w], dp[w - new_wt[i]] + new_val[i]);
        }
    }
    return dp[W];
}
