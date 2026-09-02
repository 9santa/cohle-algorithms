#pragma once
#include <bits/stdc++.h>
using namespace std;

/** 2-SAT with signed 1-indexed literals: +i means xi, -i means !xi. */
struct TwoSAT {
    int n = 0;
    vector<vector<int>> g;

    explicit TwoSAT(int variables) : n(variables), g(2 * variables) {}

    static int node(int var0, bool is_true) {
        return 2 * var0 + (is_true ? 0 : 1);
    }
    static int neg(int x) { return x ^ 1; }

    void add_or(int a, int b) {
        int va = abs(a) - 1, vb = abs(b) - 1;
        int A = node(va, a > 0);
        int B = node(vb, b > 0);
        g[neg(A)].push_back(B);
        g[neg(B)].push_back(A);
    }

    void add_implication_lit(int a, int b) { add_or(-a, b); }
    void add_true(int a) { add_or(a, a); }
    void add_false(int a) { add_or(-a, -a); }

    optional<vector<bool>> solve() const {
        int N = 2 * n;
        vector<vector<int>> rg(N);
        for (int v = 0; v < N; ++v) for (int to : g[v]) rg[to].push_back(v);

        vector<char> vis(N, false);
        vector<int> order;
        order.reserve(N);
        auto dfs1 = [&](auto&& self, int v) -> void {
            vis[v] = true;
            for (int to : g[v]) if (!vis[to]) self(self, to);
            order.push_back(v);
        };
        for (int v = 0; v < N; ++v) if (!vis[v]) dfs1(dfs1, v);

        vector<int> comp(N, -1);
        int cid = 0;
        auto dfs2 = [&](auto&& self, int v) -> void {
            comp[v] = cid;
            for (int to : rg[v]) if (comp[to] == -1) self(self, to);
        };
        reverse(order.begin(), order.end());
        for (int v : order) {
            if (comp[v] != -1) continue;
            dfs2(dfs2, v);
            ++cid;
        }

        vector<bool> ans(n);
        for (int i = 0; i < n; ++i) {
            int x = 2 * i;
            if (comp[x] == comp[x ^ 1]) return nullopt;
            ans[i] = comp[x] > comp[x ^ 1];
        }
        return ans;
    }
};
