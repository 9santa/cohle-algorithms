#include "../sparsetable/sparse_table_idempotent.h"

/*
    RMQ for any array D[] using "block + mask-in-block + sparse table over blocks"
    D[] will be Euler depths (adj diffs = +-1), but mask trick work any values
    Block size B is chosen ~log2(n), must be <= 63 (we use u64 masks)
    In-block small range RMQ(l, r) is answered in O(1) via:
        m = mask[r] & ((1ULL << len) - 1), pos = msb(m), ans = r - pos
    // Can be used with: farach-colton-bender algo
*/

// RMQ-TYPE MONOIDS ONLY (min, max, argmin, argmax...)
template<class Monoid, template<class> class SPARSE = SparseTable>
struct BlockMaskRMQ {
    using MX = Monoid;
    using X = typename MX::value_type;

    int n = 0;
    int B = 1;  // block length
    int K = 0;  // block count
    vector<X> a;  // values
    vector<u64> mask;   // mask[r] for each position r

    vector<int> blockBestPos; // for each block, global index of best
    vector<X> blockBestVal; // blockBestVal[b] = a[blockBestPos[b]]

    SPARSE<MX> st; // sparse table over blockBestVal

    BlockMaskRMQ() = default;
    explicit BlockMaskRMQ(const vector<X>& arr) { build(arr); }

    // choose better index among i, j using MX::op()
    inline int better_idx(int i, int j) const {
        return MX::op(a[i], a[j]) ? i : j;
    }

    // in-block query, assumes l and r in same block
    int queryInBlock(int l, int r) const {
        int len = r - l + 1;
        u64 bits;
        if (len >= 64) bits = ~0ULL;
        else bits = (1ULL << len) - 1ULL;
        u64 m = mask[r] & bits;
        int pos = topbit(m);
        return r - pos;
    }

    // query best among full blocks [L..R] inclusive block indices, returns global index
    int queryBlocks(int L, int R) const {
        int bi = st.query_index(L, R); // index in blockBestVal
        return blockBestPos[bi]; // global index in a
    }

    void build(const vector<X>& arr) {
        a = arr;
        n = sz(a);
        if (n == 0) {
            B = 1; K = 0;
            mask.clear(); blockBestPos.clear(); blockBestVal.clear();
            return;
        }

        // Choose max block size ~log2(n), clamp to [1..63]
        B = max(1, min(63, 31 - __builtin_clz(n)));
        K = (n + B - 1) / B;
        mask.assign(n, 0);
        blockBestPos.assign(K, 0);
        blockBestVal.resize(K);

        // Build masks per block using a monotone (Cartesian) stack
        // pop while new is strictly better than top (MX::op(new, top) == true)
        // tie behavior is controlled by MX::op
        for (int b = 0; b < K; b++) {
            int L = b * B;
            int R = min(n-1, L + B - 1);
            u64 curMask = 0;
            u64 keep = (B == 64 ? ~0ULL : ((1ULL << B) - 1ULL));
            vector<int> stk; stk.reserve(B);

            for (int i = L; i <= R; i++) {
                curMask = ((curMask << 1) | 1ULL) & keep;

                while (!stk.empty() && MX::op(a[i], a[stk.back()])) {
                    int j = stk.back(); stk.pop_back();
                    int off = i - j; // 1..B-1
                    curMask &= ~(1ULL << off); // remove popped index bit
                }
                mask[i] = curMask;
                stk.push_back(i);
            }
            // best of whole block
            int best = queryInBlock(L, R);
            blockBestPos[b] = best;
            blockBestVal[b] = a[best];
        }
        // sparse table over blocks
        st.build(blockBestVal);
    }

    // RMQ on D[l..r], returns global index of best
    int query_index(int l, int r) const {
        if (l > r) swap(l, r);
        int bl = l / B;
        int br = r / B;
        if (bl == br) return queryInBlock(l, r);

        int leftEnd = min(n-1, bl * B + B - 1);
        int rightStart = br * B;

        int ans = better_idx(queryInBlock(l, leftEnd), queryInBlock(rightStart, r));
        if (bl + 1 <= br - 1) {
            ans = better_idx(ans, queryBlocks(bl+1, br-1));
        }
        return ans;
    }

    X query_value(int l, int r) const {
        return a[query_index(l, r)];
    }
};
