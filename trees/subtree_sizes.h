#include "rooted_tree.h"

template<class G>
vi subtree_sizes(const G& g, const RootedTree& t) {
    int N = g.N;
    vi siz(N, 1);
    assert(!t.order.empty());
    for (int i = sz(t.order)-1; i >= 0; i--) {
        int v = t.order[i];
        if (v == t.parent[v]) continue;
        siz[t.parent[v]] += siz[v];
    }
    return siz;
}
