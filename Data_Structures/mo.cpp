
struct Mo {
    vector<pair<int, int>> LR;  // queries [L, R)
    void add(int L, int R) { LR.emplace_back(L, R); }

    static vector<int> get_mo_order(vector<pair<int, int>> LR) {
        int N = 1;
        for (auto &[l, r] : LR) setmax(N, l), setmax(N, r);
        int Q = sz(LR);
        if (Q == 0) return {};
        int len = sqrt(3) * N / sqrt(2 * Q);  // better block size than just sqrt(N)
        setmax(len, 1);
        vector<int> I(Q);
        iota(all(I), 0);
        // zig-zag optimization
        sort(all(I), [&](int a, int b) {
            int ablock = LR[a].first / len, bblock = LR[b].first / len;
            if (ablock != bblock) return ablock < bblock;
            return (ablock & 1) ? LR[a].second > LR[b].second : LR[a].second < LR[b].second;
        });

        auto cost = [&](int a, int b) -> int {
            return abs(LR[I[a]].first - LR[I[b]].first) + abs(LR[I[a]].second - LR[I[b]].second);
        };

        // optimize queries Manhattan distance
        for (int k = 0; k < Q-5; k++) {
            if (cost(k, k+2) + cost(k+1, k+3) < cost(k, k+1) + cost(k+2, k+3)) {
                swap(I[k+1], I[k+2]);
            }
            if (cost(k, k+3) + cost(k+1, k+4) < cost(k, k+1) + cost(k+3, k+4)) {
                swap(I[k+1], I[k+3]);
            }
        }

        return I;
    }

    template<typename F1, typename F2, typename F3, typename F4, typename FQ>
    void calc(F1 add_l, F2 add_r, F3 rm_l, F4 rm_r, FQ query) {
        auto I = get_mo_order(LR);
        int l = 0, r = 0;
        for (auto idx : I) {
            while (l > LR[idx].first) add_l(--l);
            while (r < LR[idx].second) add_r(r++);
            while (l < LR[idx].first) rm_l(l++);
            while (r > LR[idx].second) rm_r(--r);
            query(idx);
        }
    }
};
