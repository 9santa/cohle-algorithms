#include "../header.h"
#include <cstdint>


// O(S * sqrt(S) * log2(S)) with ans reconstruction
void knapsack_sqrt_reconstruction(const vector<int>& c, const vector<int>& weights, int S) {
    int n = sz(weights);

    struct Item {
        int w, v, idx;
    };

    vector<Item> items;

    // fraction items
    for (int i = 0; i < n; i++) {
        int w = weights[i], v = c[i];
        int pow2 = 1;
        while (w > 0) {
            int take = min(pow2, w);
            items.push_back({take, take * v / w, i});
            w -= take;
            pow2 *= 2;
        }
    }

    vector<int> dp(S+1, 0);
    vector<int> take(S+1, -1);  // remember index of the fractioned item we took

    for (int idx = 0; idx < sz(items); idx++) {
        auto [w, v, orig] = items[idx];
        for (int s = S; s >= w; s--) {
            if (dp[s-w] + v > dp[s]) {
                dp[s] = dp[s-w] + v;
                take[s] = orig; // store for reconstruction
            }
        }
    }

    int s = max_element(dp.begin(), dp.end()) - dp.begin();
    vector<int> res_cnt(n, 0);

    while (s > 0 && take[s] != -1) {
        int i = take[s];
        res_cnt[items[i].idx] += items[i].w;
        s -= items[i].w;
    }

    int total_cost = *max_element(all(dp));

    for (int i = 0; i < n; i++) {
        if (res_cnt[i] > 0) {
            cout << "Item: " << i << ": " << res_cnt[i] << " pieces (quantity)\n";
        }
    }
}

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

// O(S * sqrt(S) * log2(S))
int knapsack_sqrt(const vector<int>& c, const vector<int>& weights, int S) {
    int n = sz(weights);

    vector<pair<int, int>> items;

    for (int i = 0; i < n; i++) {
        int w = weights[i], v = c[i];
        int pow2 = 1;
        while (w > 0) {
            int take = min(pow2, w);
            items.push_back({take, take * v / w});
            w -= take;
            pow2 *= 2;
        }
    }

    vector<int> dp(S+1, 0);
    for (auto [w, v] : items) {
        for (int s = S; s >= w; s--) {
            dp[s] = max(dp[s], dp[s-w] + v);
        }
    }

    return *max_element(all(dp));
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

// Costs, weights. Goal: maximize the total cost
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

// Space optimized knapsack. O(W) memory
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

// 2*W memory. Alternating arrays approach. Keeping only 2 rows.
int knapsack_mem_optimized1(const vector<int>& c, const vector<int>& w, int W) {
    int n = sz(w);
    vector<int> odp(W+1, 0), ndp(W+1, 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= W; j++) {
            ndp[j] = odp[j];
            if (w[i-1] <= j)
                ndp[j] = max(ndp[j], odp[j - w[i-1]] + c[i-1]);
        }
        swap(odp, ndp);
    }
    return odp[W];
}

// O(n * W)
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
