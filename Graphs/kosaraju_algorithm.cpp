#include <bits/stdc++.h>
using namespace std;

#define all(x) x.begin(), x.end()
#define rall(x) x.rbegin(), x.rend()
#define eb emplace_back
#define debug(x) std::cout << __FUNCTION__ << ":" << __LINE__ << " " << #x << " = " << x << std::endl

using u128 = unsigned __int128;
using i128 = __int128;
using u64  = unsigned long long;
using i64  = long long;
using ll   = long long;
using u32  = unsigned int;
using u8   = uint8_t;
using u16  = uint16_t;
using ld   = long double;

// pairs
using pi = std::pair<int,int>;
#define mp make_pair
#define F first
#define S second

// containers
#define sz(x) int((x).size())
#define pb push_back
#define rsz resize
#define ft front()
#define bk back()

// loops
#define FOR(i,a,b) for (int i = (a); i < (b); i++)
#define F0R(i,a) FOR(i,0,a)
#define ROF(i,a,b) for (int i = (b)-1; i >= (a); i--)
#define R0F(i,a) ROF(i,0,a)
#define REP(a) F0R(_,a)

template<typename T>
using V = std::vector<T>;
template<typename T, size_t SZ>
using AR = std::array<T,SZ>;
using vi = V<int>;
using vb = V<bool>;
using vpi = V<pi>;

template<class T>
constexpr T infty = 0;
template<>
constexpr int infty<int> = 1'010'000'000;
template<>
constexpr ll infty<ll> = 2'020'000'000'000'000'000;

// first el is the smallest
template<typename T> using min_heap=std::priority_queue<T, std::vector<T>, std::greater<T>>;
// first el is the largest
template<typename T> using max_heap=std::priority_queue<T, std::vector<T>, std::less<T>>;

template<typename T> void print(std::vector<T> a) { for (auto x : a) std::cout << x << " "; std::cout << "\n"; }
template<typename T> void print(std::set<T> a) { for (auto x : a) std::cout << x << " "; std::cout << "\n"; }
template<typename T> void print(std::unordered_set<T> a) { for (auto x : a) std::cout << x << " "; std::cout << "\n"; }
template<typename T> void print(T&& x) { std::cout << x << "\n"; }
template<typename T, typename... S> void print(T&& x, S&&... y) { std::cout << x << " "; print(y...); }

template<typename T, typename S> constexpr void setmin(T& x, const S y) { if (y < x) x = y; }
template<typename T, typename S> constexpr void setmax(T& x, const S y) { if (y > x) x = y; }

/**
 * Description: Kosaraju's Algorithm, DFS twice to generate
 * strongly connected components in topological order. a,b
 * in same component if both a to b and b to a paths exist.
 * Time: O(N+M)
 */
struct SCCk {
    int N; V<vi> adj, radj;
    vi topo, comp, comps; V<bool> vis;
    void init(int _N) {
        N = _N;
        adj.resize(N), radj.resize(N), comp.assign(N, -1), vis.resize(N);
    }
    void dfs(int x) {
        vis[x] = 1;
        for (auto y : adj[x]) if (!vis[y]) dfs(y);
        topo.pb(x);
    }
    void dfs2(int x, int v) {
        comp[x] = v;
        for (auto y : radj[x]) if (comp[y] == -1) dfs2(y, v);
    }
    void gen() {
        F0R(i, N) if (!vis[i]) dfs(i);
        reverse(all(topo));
        for (auto x : topo) if (comp[x] == -1) dfs2(x, x), comps.pb(x);
    }
};


void print_sccs(const SCCk& kos) {
    map<int, vector<int>> scc_groups;

    for (int i = 1; i < kos.N; i++) {
        int root_id = kos.comp[i];
        if (root_id != -1) {
            scc_groups[root_id].push_back(i);
        }
    }

    int scc_count = 1;
    for (int root_id : kos.comps) {
        cout << "SCC " << scc_count++ << " (Root: " << root_id << "): {";
        const auto& vertices = scc_groups[root_id];
        for (int i = 0; i < (int)vertices.size(); i++) {
            cout << vertices[i] << (i == vertices.size()-1 ? "" : ", ");
        }
        cout << "}\n";
    }
    cout << "Total SCCs found: " << kos.comps.size() << "\n";
    print("\n");
}

// tests
int main(void) {
    ios::sync_with_stdio(0);
    cin.tie(0);

    vector<vector<int>> graph0 = {
        {},
        {2, 6},
        {3, 4},
        {1, 7},
        {5},
        {4},
        {3, 7},
        {5}
    };

    vector<vector<int>> graph1 = {
        {},           // 0 unused (1-indexed)
        {2},          // 1 -> 2
        {3, 4},       // 2 -> 3, 2 -> 4
        {1},          // 3 -> 1
        {5},          // 4 -> 5
        {6},          // 5 -> 6
        {4, 7},       // 6 -> 4, 6 -> 7
        {8},          // 7 -> 8
        {9},          // 8 -> 9
        {7}           // 9 -> 7
    };

    vector<vector<int>> graph2 = {
        {},           // 0 unused
        {2},          // 1 → 2
        {3},          // 2 → 3  
        {1},          // 3 → 1 (cycle: 1-2-3)
        {1, 5},       // 4 → 1, 4 → 5
        {6},          // 5 → 6
        {4},          // 6 → 4 (cycle: 4-5-6)
        {6, 8},       // 7 → 6, 7 → 8
        {9},          // 8 → 9
        {7}           // 9 → 7 (cycle: 7-8-9)
    };

    vector<vector<int>> graph3 = {
        {},           // 0 unused (1-indexed)
        {2, 3},       // 1 -> 2, 1 -> 3
        {4},          // 2 -> 4
        {2},          // 3 -> 2
        {5, 6},       // 4 -> 5, 4 -> 6
        {7},          // 5 -> 7
        {5},          // 6 -> 5 (creates cycle 5-6)
        {8},          // 7 -> 8
        {9},          // 8 -> 9
        {7}           // 9 -> 7 (creates cycle 7-8-9)
    };

    SCCk kos0, kos1, kos2, kos3;
    kos0.init(8), kos1.init(10), kos2.init(10), kos3.init(10);

    for (size_t u = 1; u < graph0.size(); u++) {
        for (int v : graph0[u]) {
            kos0.adj[u].pb(v);
            kos0.radj[v].pb(u);
        }
    }

    for (size_t u = 1; u < graph1.size(); u++) {
        for (int v : graph1[u]) {
            kos1.adj[u].pb(v);
            kos1.radj[v].pb(u);
        }
    }
    for (size_t u = 1; u < graph2.size(); u++) {
        for (int v : graph2[u]) {
            kos2.adj[u].pb(v);
            kos2.radj[v].pb(u);
        }
    }
    for (size_t u = 1; u < graph3.size(); u++) {
        for (int v : graph3[u]) {
            kos3.adj[u].pb(v);
            kos3.radj[v].pb(u);
        }
    }

    kos0.gen(), kos1.gen(), kos2.gen(), kos3.gen();


    print_sccs(kos0);
    print_sccs(kos1);
    print_sccs(kos2);
    print_sccs(kos3);


    return 0;
}
