// #pragma GCC optimize("Ofast,inline,unroll-loops")
// #pragma GCC target("avx2,popcnt")

#include <algorithm>
#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

#define all(x) x.begin(), x.end()
#define rall(x) x.rbegin(), x.rend()
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
#define eb emplace_back
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
using vl = V<ll>;
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


// binary comparator (set, map)
struct cmp{bool operator() (int l,int r) const {return l > r;}};

constexpr int MOD = 1e9+7;
constexpr int MAXN = 100005;

// with pseudo-topsort first
namespace first {
vi topo;
vb used;
V<vi> graph;

void dfs(int v) {
    used[v] = true;
    for (auto u : graph[v]) {
        if (!used[u]) {
            dfs(u);
        }
    }
    topo.pb(v);
}

void solve(void) {
    int n, m; cin >> n >> m;
    graph.resize(n);
    used.assign(n, false);
    while (m--) {
        int u, v; cin >> u >> v;
        u--, v--;
        graph[u].pb(v);
    }

    F0R(i, n) {
        if (!used[i]) dfs(i);
    }

    vi dp(n, 0);

    reverse(all(topo));
    for (auto u : topo) {
        for (auto v : graph[u]) {
            dp[v] = max(dp[v] , dp[u]+1);
        }
    }

    int ans = *max_element(all(dp));
    print(ans);

}
} // namespace first

// in 1 dfs without explicit topsort
namespace second {
vi g[MAXN];
int dp[MAXN];
int parent[MAXN];

int dfs(int u) {
    if (dp[u] != -1) return dp[u];

    dp[u] = 1; // atleast 1 length path from this vertex (vertex itself)
    parent[u] = -1;

    for (int v : g[u]) {
        int candi = 1 + dfs(v);
        if (candi > dp[u]) {
            dp[u] = candi;
            parent[u] = v;
        }
    }
    return dp[u];
}

int main() {
    int n;
    memset(dp, -1, sizeof(dp));
    int max_length = 0;
    int startv = -1;
    for (int i = 1; i <= n; i++) {
        int length = dfs(i);
        if (length > max_length) {
            max_length = length;
            startv = i;
        }
    }
    return 0;
}

} // namespace second
