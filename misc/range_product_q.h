#include "../header.h"

using int64 = long long;
using i128  = __int128_t;
using u128  = __uint128_t;

constexpr int MOD = 998244353;

template<typename T>
constexpr T binpow(T a, u64 b) {
    T res = 1;
    while (b > 0) {
        if (b&1) res *= a;
        a *= a;
        b >>= 1;
    }
    return res;
}

template<u32 P>
constexpr u32 mulMod(u32 a, u32 b) {
    return u64(a) * b % P;
}

/* this is some tricky stuff, intentionally overflows u64
   but recovers the correct value using lond double approx */
template<u64 P>
constexpr u64 mulMod(u64 a, u64 b) {
    u64 res = a * b - u64(1.L * a * b / P - 0.5L) * P;
    res %= P;
    return res;
}

// signed modulo fix
constexpr i64 safeMod(i64 x, i64 mod) {
    x %= mod;
    if (x < 0) x += mod;
    return x;
}

static i64 egcd(i64 a, i64 b, i64 &x, i64 &y) {
    // returns g=gcd(a,b), and finds x,y such that ax+by=g
    if (b == 0) { x = 1; y = 0; return a; }
    i64 x1, y1;
    i64 g = egcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

static i64 modInv(i64 a, i64 mod) {
    // inverse exists if gcd(a, mod)=1
    i64 x, y;
    i64 g = egcd(a, mod, x, y);
    if (g != 1) {
        throw runtime_error("mod_inv: inverse does not exist");
    }
    return safeMod(x, mod);
}

// returns pair {prime, power}
vector<pair<ll, ll>> factorize(ll n) {
    vector<pair<ll, ll>> primes;
    for (ll i = 2; i*i <= n; i++) {
        ll exp = 0;
        while (n%i == 0) {
            exp++;
            n /= i;
        }
        if (exp > 0) primes.pb({i, exp});
    }
    if (n > 1) primes.pb({n, 1});
    return primes;
}


// ------------------------------
// CRT for pairwise coprime moduli
// Combine x ≡ a1 (mod m1), x ≡ a2 (mod m2), gcd(m1,m2)=1
// ------------------------------
static pair<i64,i64> crt_merge(u64 a1, u64 m1, u64 a2, u64 m2) {
    // returns (a, m) where x ≡ a (mod m), m=m1*m2
    // t = (a2-a1) * inv(m1 mod m2) mod m2
    i64 inv = modInv(safeMod(m1, m2), m2);
    u64 t = safeMod(a2 - a1, m2);
    t = mulMod<m2>(t, inv);

    // a = a1 + m1 * t
    i128 a = (i128)a1 + (i128)m1 * (i128)t;
    int64 m = (int64)((u128)m1 * (u128)m2);
    int64 res = (int64)(a % m);
    if (res < 0) res += m;
    return {res, m};
}

// ============================================================
// Range Product Mod M using prime-power decomposition + prefixes
// ============================================================
struct RangeProductMod {
    int n = 0;
    int64 M = 1;

    // prime powers: M = Π mod_i, where mod_i = p_i^e_i
    vector<int64> p;        // prime
    vector<int>   e;        // exponent
    vector<int64> mod_i;    // p^e

    // prefix_zero[i] = number of zeros in a[0..i)
    vector<int> pref_zero;

    // For each i-th prime power:
    // pref_exp[i][j] = Σ v_p(a[t]) for t in [0..j)
    // pref_prod[i][j] = Π reduced_part_p(a[t]) for t in [0..j) mod mod_i
    // inv_pref_prod[i][j] = (pref_prod[i][j])^{-1} mod mod_i
    vector<vector<int64>> pref_exp;
    vector<vector<int64>> pref_prod;
    vector<vector<int64>> inv_pref_prod;

    // reduced_part[i][j] = a[j] with all factors p_i removed, taken mod mod_i (so invertible)
    // (We store it to build inv_pref_prod in O(n) with only 1 inverse per prime power.)
    vector<vector<int64>> reduced_part;

    // pow_p[i][k] = p_i^k mod mod_i for k=0..e_i (small table)
    vector<vector<int64>> pow_p;

    RangeProductMod() = default;

    explicit RangeProductMod(const vector<int64>& a, int64 mod) { build(a, mod); }

    void build(const vector<int64>& a, int64 mod) {
        n = (int)a.size();
        M = mod;
        if (M == 1) {
            // everything is 0 mod 1
            pref_zero.assign(n+1, 0);
            p.clear(); e.clear(); mod_i.clear();
            pref_exp.clear(); pref_prod.clear(); inv_pref_prod.clear();
            reduced_part.clear(); pow_p.clear();
            return;
        }

        auto f = factorize_prime_powers(M);
        int k = (int)f.size();
        p.resize(k); e.resize(k); mod_i.resize(k);

        for (int i = 0; i < k; i++) {
            p[i] = f[i].first;
            e[i] = f[i].second;
            mod_i[i] = ipow_int64(p[i], e[i]);
        }

        // zeros prefix
        pref_zero.assign(n+1, 0);
        for (int i = 0; i < n; i++) pref_zero[i+1] = pref_zero[i] + (a[i] == 0);

        // allocate tables
        pref_exp.assign(k, vector<int64>(n+1, 0));
        pref_prod.assign(k, vector<int64>(n+1, 1));
        inv_pref_prod.assign(k, vector<int64>(n+1, 1));
        reduced_part.assign(k, vector<int64>(n, 1));
        pow_p.assign(k, {});

        // precompute p^t mod p^e (t=0..e)
        for (int i = 0; i < k; i++) {
            pow_p[i].assign(e[i] + 1, 1);
            for (int t = 1; t <= e[i]; t++) {
                pow_p[i][t] = mulMod(pow_p[i][t-1], p[i], mod_i[i]);
            }
        }

        // Build prefix exponent sums and prefix reduced products.
        for (int i = 0; i < k; i++) {
            const int64 prime = p[i];
            const int64 modpp = mod_i[i];

            pref_prod[i][0] = 1;
            pref_exp[i][0]  = 0;

            for (int j = 0; j < n; j++) {
                pref_exp[i][j+1] = pref_exp[i][j];
                pref_prod[i][j+1] = pref_prod[i][j];

                if (a[j] == 0) {
                    // We will short-circuit queries containing zeros -> answer 0 mod M.
                    // To keep reduced products invertible, treat reduced_part as 1 and exp as 0 here.
                    reduced_part[i][j] = 1;
                    continue;
                }

                // Compute v_p(a[j]) and reduced value = a[j] / p^v
                int64 x = a[j];
                int64 v = 0;

                // remove prime factors; note: works with negative too since x%prime==0 still true
                while (x % prime == 0) {
                    x /= prime;
                    v++;
                    // v can be large if a[j]==0, but we handled zero above
                }

                // x is now coprime to prime => invertible mod p^e
                int64 red = safeMod(x, modpp);

                reduced_part[i][j] = red;
                pref_exp[i][j+1] = pref_exp[i][j] + v;
                pref_prod[i][j+1] = mulMod(pref_prod[i][j], red, modpp);
            }

            // Build inverse of every prefix product in O(n) using one modular inverse:
            // inv_pref_prod[i][n] = inv(pref_prod[i][n])
            // inv_pref_prod[i][j] = inv_pref_prod[i][j+1] * reduced_part[i][j]  (mod modpp)
            //
            // Proof:
            // pref_prod[j+1] = pref_prod[j] * red[j]
            // inv(pref_prod[j]) = inv(pref_prod[j+1]) * red[j]
            //
            // (commutative multiplication; here we are in Z/(p^e), multiplication is commutative)
            inv_pref_prod[i][n] = mod_inv(pref_prod[i][n], modpp);
            for (int j = n - 1; j >= 0; j--) {
                inv_pref_prod[i][j] = mulMod(inv_pref_prod[i][j+1], reduced_part[i][j], modpp);
            }
        }
    }

    // Product of a[l..r) modulo M
    int64 prod(int l, int r) const {
        if (M == 1) return 0;
        if (l < 0) l = 0;
        if (r > n) r = n;
        if (l >= r) return 1 % M;

        // If any zero in range => product = 0 mod M
        if (pref_zero[r] - pref_zero[l] > 0) return 0;

        const int k = (int)p.size();
        vector<int64> rem(k);

        // Compute residue modulo each prime power p_i^{e_i}
        for (int i = 0; i < k; i++) {
            int64 modpp = mod_i[i];

            int64 V = pref_exp[i][r] - pref_exp[i][l];
            if (V >= e[i]) {
                rem[i] = 0; // divisible by p^e -> 0 mod p^e
                continue;
            }

            // Reduced part product:
            // R = pref_prod[r] / pref_prod[l] mod modpp
            //   = pref_prod[r] * inv(pref_prod[l]) mod modpp
            int64 R = mulMod(pref_prod[i][r], inv_pref_prod[i][l], modpp);

            // Multiply back p^V
            int64 ans_i = mulMod(R, pow_p[i][(int)V], modpp);
            rem[i] = ans_i;
        }

        // CRT combine all rem[i] (mod mod_i[i]) into modulo M
        int64 a = 0;
        int64 m = 1;
        for (int i = 0; i < (int)p.size(); i++) {
            auto merged = crt_merge(a, m, rem[i], mod_i[i]);
            a = merged.first;
            m = merged.second;
        }
        // m should equal M now
        return a;
    }
};

// ------------------------------
// Example usage
// ------------------------------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    long long M;
    cin >> n >> q >> M;
    vector<long long> a(n);
    for (int i = 0; i < n; i++) cin >> a[i];

    RangeProductMod rpm(a, M);

    while (q--) {
        int l, r;
        cin >> l >> r; // assume [l,r) 0-indexed
        cout << rpm.prod(l, r) << "\n";
    }
    return 0;
}
