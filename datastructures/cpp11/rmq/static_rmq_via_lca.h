
// --- Static RMQ on array via LCA ---
// array A -> Cartesian tree -> LCA
// LCA -> RMQ on Euler-depth array (adj diff +-1)
// +-1 RMQ via Four Russians OR mask[i] approach -> O(1) queries
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
