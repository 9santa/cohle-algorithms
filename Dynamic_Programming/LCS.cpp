// #pragma GCC optimize("Ofast,inline,unroll-loops")
// #pragma GCC target("avx2,popcnt")

#include <bits/stdc++.h>
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


/* ---solution--- */
void solve(void) {
    string s, t;
    cin >> s >> t;

    int n = sz(s), m = sz(t);
    V<vi> dp(n+1, vi(m+1));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (s[i] == t[j]) dp[i+1][j+1] = dp[i][j] + 1;
            else dp[i+1][j+1] = max(dp[i][j+1], dp[i+1][j]);
        }
    }

    string res;
    int i = n, j = m;
    while (i > 0 && j > 0) {
        if (s[i-1] == t[j-1]) {
            res.pb(s[i-1]);
            i--; j--;
        } else if (dp[i-1][j] >= dp[i][j-1]) {
            i--;
        } else j--;
    }

    reverse(all(res));
    print(res);

}

signed main(void) {
    clock_t z = clock();
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // int t; std::cin >> t;
    // while(t--) solve();
    solve();

    std::cerr << "\nRun Time : " << std::fixed << std::setprecision(6) << ((double)(clock() - z) / CLOCKS_PER_SEC) << '\n';


    return 0;
}
