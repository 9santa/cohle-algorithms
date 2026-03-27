#pragma once

#include "../core.h"

// --- RMQ +-1 ---
// Static RMQ in O(1) per query with Four Russians, O(sz(euler)) preprocessing
// Static RMQ -> Static LCA -> Static RMQ +-1 -> Four Russians -> O(1)
struct PlusMinusOneRMQ {
    int n = 0;     // length Euler depth array
    int B = 1;     // block size
    int K = 0;     // number of blocks
    int types = 1; // 2^(B-1)

    vector<int> D;           // the array
    vector<int> blockType;   // per block, bitmask of +/-1 steps
    vector<int> blockMinPos; // per block, global position of min inside block

    // micro[(t*B + i)*B + j] = argmin in [i..j] inside a block of type t
    vector<u8> micro;

    // sparse table over blocks, storing global positions in D
    vector<int> lg;
    vector<vector<int>> st;

    PlusMinusOneRMQ() = default;
    explicit PlusMinusOneRMQ(const vector<int>& depths) { build(depths); }

    static int floor_log2(int x) {
        int res = 0;
        while ((1 << (res + 1)) <= x) ++res;
        return res;
    }

    inline int better(int pos1, int pos2) const {
        // compare by D value, tie by smaller index (leftmost)
        if (D[pos1] != D[pos2]) return (D[pos1] < D[pos2]) ? pos1 : pos2;
        return min(pos1, pos2);
    }

    // helper for access
    inline u8 microAt(int t, int i, int j) const {
        return micro[(t * B + i) * B + j];
    }

    void build(const vector<int>& depths) {
        D = depths;
        n = sz(D);
        if (n == 0) {
            B = 1;
            K = 0;
            types = 1;
            return;
        }

        // choose B ~ 1/2 log2(n)
        int logn = floor_log2(n);
        B = max(1, min(16, logn / 2));
        types = 1 << max(0, B - 1);

        K = (n + B - 1) / B; // cnt of blocks
        blockType.assign(K, 0);
        blockMinPos.assign(K, 0);

        // precompute micro table for all types
        micro.assign((size_t)types * B * B, 0);
        vector<int> h(B);
        for (int t = 0; t < types; t++) {
            h[0] = 0;
            for (int k = 0; k < B - 1; k++) {
                int step = ((t >> k) & 1) ? +1 : -1;
                h[k + 1] = h[k] + step;
            }
            for (int i = 0; i < B; i++) {
                int best = i;
                int bestval = h[i];
                for (int j = i; j < B; j++) {
                    if (h[j] < bestval) {
                        bestval = h[j];
                        best = j;
                    }
                    micro[(t * B + i) * B + j] = (u8)best;
                }
            }
        }

        // compute block types and block mins
        for (int b = 0; b < K; b++) {
            int start = b * B;
            int end = min(n - 1, start + B - 1);

            int mask = 0;
            for (int i = 0; i < B - 1; i++) {
                int pos = start + i;
                if (pos + 1 >= n) break;
                if (D[pos + 1] - D[pos] == +1) mask |= (1 << i);
            }
            blockType[b] = mask;

            int j = end - start;
            int off = microAt(mask, 0, j);
            blockMinPos[b] = start + off;
        }

        // build sparse table over block minima
        lg.assign(K + 1, 0);
        for (int i = 2; i <= K; i++) lg[i] = lg[i / 2] + 1;

        int P = lg[K] + 1;
        st.assign(P, vector<int>(K));
        for (int i = 0; i < K; i++) st[0][i] = blockMinPos[i];

        for (int p = 1; p < P; p++) {
            int len = 1 << p;
            int half = len >> 1;
            for (int i = 0; i + len <= K; i++) {
                st[p][i] = better(st[p - 1][i], st[p - 1][i + half]);
            }
        }
    }

    int queryBlocks(int L, int R) const {
        // inclusive block indices, requires L <= R
        assert(L <= R);
        int len = R - L + 1;
        int p = lg[len];
        int a = st[p][L];
        int b = st[p][R - (1 << p) + 1];
        return better(a, b);
    }

    int query(int l, int r) const {
        // inclusive positions in D
        if (l > r) swap(l, r);
        int bl = l / B;
        int br = r / B;
        if (bl == br) {
            int i = l % B;
            int j = r % B;
            int off = microAt(blockType[bl], i, j);
            return bl * B + off;
        }

        // left tail: [l .. end of left block]
        int leftPos = bl * B + microAt(blockType[bl], l % B, B - 1);
        // right tail: [start of right block .. r]
        int rightPos = br * B + microAt(blockType[br], 0, r % B);

        int bestPos = better(leftPos, rightPos);

        // middle full blocks
        if (bl + 1 <= br - 1) {
            int midPos = queryBlocks(bl + 1, br - 1);
            bestPos = better(bestPos, midPos);
        }
        return bestPos;
    }
};
