#include "../header.h"

template<class G>
vi out_degrees(const G& g) {
    vi out(g.N);
    for (int v = 0; v < g.N; v++) out[v] = g.indptr[v+1] - g.indptr[v];
    return out;
}

template<class G>
vi in_degrees_from_csr(const G& g) {
    vi in(g.N, 0);
    for (int v = 0; v < g.N; v++) {
        for (auto& e : g[v]) in[e.to]++;
    }
    return in;
}

template<class G>
vi in_degrees_from_edges(const G& g) {
    vi in(g.N, 0);
    for (auto& e : g.edges) in[e.to]++;
    return in;
}
