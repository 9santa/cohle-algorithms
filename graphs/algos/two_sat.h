#include "scc_kosaraju.h"
#include <optional>

// 2-SAT with literals encoded as +/- (1-indexed variables)
// +i means x_i if true, -i means x_i is false
/** 2-SAT solver using SCC on implication graph. Space: O(n + clauses). */
struct TwoSAT {
    int n = 0;
    Graph<int, true> imp; // implication graph on 2*n nodes

    explicit TwoSAT(int variables) : n(variables), imp(2 * variables) {}

    /** Returns node id for zero-indexed variable and truth value. Time: O(1). */
    static int node(int var0, bool is_true) {
        return 2 * var0 + (is_true ? 0 : 1);
    }
    /** Returns node id of negated literal node x. Time: O(1). */
    static int neg(int x) { return x ^ 1; }

    // add clause (a OR b), where a and b are signed literals: +/- (1..n)
    /** Adds clause (a OR b), literals encoded as +/- 1-indexed vars. Time: O(1). */
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
    /** Adds implication a -> b. Time: O(1). */
    void add_implication_lit(int a, int b) {
        // a -> b is (!a OR b)
        add_or(-a, b);
    }
    /** Forces literal a to be true. Time: O(1). */
    void add_true(int a) { add_or(a, a); }
    /** Forces literal a to be false. Time: O(1). */
    void add_false(int a) { add_or(-a, -a); }

    // Returns assignment if possible, else nullopt
    /** Solves the formula and returns an assignment if satisfiable. Time: O(n + clauses). */
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
