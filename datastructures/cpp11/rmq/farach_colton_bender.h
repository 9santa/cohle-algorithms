#include "../header.h"

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
        while ((1 << (res+1)) <= x) ++res;
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
            B = 1; K = 0; types = 1;
            return;
        }

        // choose B ~ 1/2 log2(n)
        int logn = floor_log2(n);
        B = max(1, min(16, logn / 2));
        types = 1 << max(0, B-1);

        K = (n + B - 1) / B; // cnt of blocks
        blockType.assign(K, 0);
        blockMinPos.assign(K, 0);

        // precompute micro table for all types
        micro.assign((size_t)types * B * B, 0);
        vector<int> h(B);
        for (int t = 0; t < types; t++) {
            h[0] = 0;
            for (int k = 0; k < B-1; k++) {
                int step = ((t>>k) & 1) ? +1 : -1;
                h[k+1] = h[k] + step;
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
            int end = min(n-1, start + B - 1);

            int mask = 0;
            for (int i = 0; i < B-1; i++) {
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
        lg.assign(K+1, 0);
        for (int i = 2; i <= K; i++) lg[i] = lg[i/2] + 1;

        int P = lg[K] + 1;
        st.assign(P, vector<int>(K));
        for (int i = 0; i < K; i++) st[0][i] = blockMinPos[i];

        for (int p = 1; p < P; p++) {
            int len = 1 << p;
            int half = len >> 1;
            for (int i = 0; i + len <= K; i++) {
                st[p][i] = better(st[p-1][i], st[p-1][i+half]);
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
        int leftPos = bl * B + microAt(blockType[bl], l % B, B-1);
        // right tail: [start of right block .. r]
        int rightPos = br * B + microAt(blockType[br], 0, r % B);

        int bestPos = better(leftPos, rightPos);

        // middle full blocks
        if (bl + 1 <= br - 1) {
            int midPos = queryBlocks(bl+1, br-1);
            bestPos = better(bestPos, midPos);
        }
        return bestPos;
    }
};

// --- Static RMQ on array via LCA ---
// array A -> Cartesian tree -> LCA
// LCA -> RMQ on Euler-depth array (adj diff +-1)
// +-1 RMQ via Four Russians -> O(1) queries
struct FastRMQ {
    int n = 0;
    vector<ll> a;

    // Cartesian tree
    vector<int> parent, lc, rc; // lc - left child, rc - right child
    int root = -1;

    // Euler rout for LCA
    vector<int> euler;  // node ids
    vector<int> depth;  // depth at each euler position
    vector<int> first;  // first occ of each node in euler

    PlusMinusOneRMQ rmqDepth;  // RMQ over depth[]

    FastRMQ() = default;
    explicit FastRMQ(const vector<ll>& arr) { build(arr); }

    inline bool keyLess(int i, int j) const {
        if (a[i] != a[j]) return a[i] < a[j];
        return i < j;
    }

    void build_cartesian() {
        parent.assign(n, -1);
        lc.assign(n, -1);
        rc.assign(n, -1);

        vector<int> st;
        st.reserve(n);

        for (int i = 0; i < n; i++) {
            int last = -1;
            while (!st.empty() && keyLess(i, st.back())) {
                last = st.back();
                st.pop_back();
            }
            if (!st.empty()) {
                parent[i] = st.back();
                rc[st.back()] = i;
            }
            if (last != -1) {
                parent[last] = i;
                lc[i] = last;
            }
            st.push_back(i);
        }

        root = -1;
        for (int i = 0; i < n; i++) {
            if (parent[i] == -1) { root = i; break; }
        }
    }

    void build_euler_tour() {
        euler.clear(), depth.clear();
        first.assign(n, -1);
        euler.reserve(2*n-1);
        depth.reserve(2*n-1);

        // build children lists
        vector<array<int, 2>> kids(n);
        vector<int> kidCnt(n, 0);
        for (int v = 0; v < n; v++) {
            kidCnt[v] = 0;
            if (lc[v] != -1) kids[v][kidCnt[v]++] = lc[v];
            if (rc[v] != -1) kids[v][kidCnt[v]++] = rc[v];
        }

        auto record = [&](int v, int d) {
            if (first[v] == -1) first[v] = sz(euler);
            euler.push_back(v);
            depth.push_back(d);
        };

        // iterative DFS for Euler tour
        vector<int> stV, stI, stD;
        stV.reserve(n), stI.reserve(n), stD.reserve(n);
        stV.push_back(root);
        stI.push_back(0);
        stD.push_back(0);
        record(root, 0);

        while (!stV.empty()) {
            int v = stV.back();
            int &i = stI.back();
            int d = stD.back();

            if (i < kidCnt[v]) {
                int c = kids[v][i++];
                stV.push_back(c);
                stI.push_back(0);
                stD.push_back(d+1);
                record(c, d+1);
            } else {
                stV.pop_back();
                stI.pop_back();
                stD.pop_back();
                if (!stV.empty()) {
                    record(stV.back(), stD.back());
                }
            }
        }
    }

    void build(const vector<ll>& arr) {
        a = arr;
        n = sz(a);
        if (n == 0) return;
        build_cartesian();
        build_euler_tour();
        rmqDepth.build(depth);
    }

    int lcaIndex(int u, int v) const {
        int L = first[u];
        int R = first[v];
        int pos = rmqDepth.query(L, R);
        return euler[pos];
    }

    int rmqIndex(int l, int r) const {
        if (l > r) swap(l, r);
        return lcaIndex(l, r);
    }

    ll rmqValue(int l, int r) const {
        return a[rmqIndex(l, r)];
    }
};
