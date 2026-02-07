#include "../../header.h"
#include "../../datastructures/dsu/dsu.cpp"

inline vi find_redundant_edge(const V<vi>& edges) {
    int mx = 0;
    for (auto& e : edges) mx = max(mx, max(e[0], e[1]));
    DSU dsu(mx + 1);

    for (auto& e : edges) {
        if (dsu.isConnected(e[0], e[1])) return e;
        dsu.unionSets(e[0], e[1]);
    }
    return {};
}
