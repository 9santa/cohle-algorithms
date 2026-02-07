#include "scc_kosaraju.h"
#include <optional>

// 2-SAT with literals encoded as +/- (1-indexed variables)
// +i means x_i if true, -i means x_i is false
struct TwoSAT {
    int n = 0;
    Graph<int, true> imp; // implication graph on 2*n nodes

    explicit TwoSAT(int variables) : n(variables), imp(2 * variables) {}

    static int node(int var0, bool is_true) {
        return 2 * var0 + (is_true ? 0 : 1);
    }
    static int neg(int x) { return x ^ 1; }

    // add clause (a OR b), where a and b are signed literals: +/- (1..n)
    void add_or(int a, int b) {
        int va = std::abs(a) - 1;
        int vb = std::abs(b) - 1;
        bool ta = (a > 0);
        bool tb = (b > 0);

        int A = node(va, ta);
        int B = node(vb, tb);
        imp.add_edge(neg(A), B);
        imp.add_edge(neg(B), A);
    }

    // helpers
    void add_implication_lit(int a, int b) {
        // a -> b is (!a OR b)
        add_or(-a, b);
    }
    void add_true(int a) { add_or(a, a); }
    void add_false(int a) { add_or(-a, -a); }

    // Returns assignment if possible, else nullopt
    std::optional<V<bool>> solve() {
        imp.build();
        auto scc = scc_kosaraju(imp);

        V<bool> ans(n, false);
        for (int i = 0; i < n; i++) {
            int x = 2 * i;
            if (scc.comp[x] == scc.comp[x ^ 1]) return std::nullopt;
            ans[i] = (scc.comp[x] > scc.comp[x ^ 1]);
        }
        return ans;
    }
};
