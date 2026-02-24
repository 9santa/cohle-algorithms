#include "../../header.h"

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


// generic modular integer class
template<std::unsigned_integral U, U P>
class ModIntBase {
private:
    U x;

public:
    constexpr ModIntBase() : x(0) {}
    template<std::unsigned_integral T>
    constexpr ModIntBase(T x_) : x(x_ % mod()) {} // reduce modulo P

    // handle negative numbers
    template<std::signed_integral T>
    constexpr ModIntBase(T x_) {
        using S = std::make_signed_t<U>; // convert modulus to signed
        S v = x_ % S(mod()); // signed % signed
        if (v < 0) {
            v += mod();
        }
        x = v; // safe now
    }

    constexpr static U mod() {
        return P;
    }

    constexpr U val() const {
        return x;
    }

    constexpr ModIntBase operator-() const {
        ModIntBase res;
        res.x = (x == 0 ? 0 : mod() - x);
        return res;
    }

    constexpr ModIntBase &operator*=(const ModIntBase &rhs) & {
        x = mulMod<mod()>(x, rhs.val());
        return *this;
    }

    constexpr ModIntBase &operator+=(const ModIntBase &rhs) & {
        x += rhs.val();
        if (x >= mod()) x -= mod();
        return *this;
    }

    constexpr ModIntBase &operator-=(const ModIntBase &rhs) & {
        x -= rhs.val();
        if (x >= mod()) x += mod();
        return *this;
    }

    constexpr ModIntBase &operator/=(const ModIntBase &rhs) & {
        return *this *= rhs.inv();
    }

    // modular inverse (fermat's little theorem), mod() has to be prime
    constexpr ModIntBase inv() const {
        return binpow(*this, mod() - 2);
    }

    friend constexpr ModIntBase operator+(ModIntBase lhs, const ModIntBase& rhs) {
        lhs += rhs;
        return lhs;
    }


    friend constexpr ModIntBase operator-(ModIntBase lhs, const ModIntBase& rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend constexpr ModIntBase operator*(ModIntBase lhs, const ModIntBase& rhs) {
        lhs *= rhs;
        return lhs;
    }

    friend constexpr ModIntBase operator/(ModIntBase lhs, const ModIntBase& rhs) {
        lhs /= rhs;
        return lhs;
    }

    friend constexpr std::istream &operator>>(std::istream &is, ModIntBase &a) {
        i64 i; is >> i; a = i; return is;
    }

    friend constexpr std::ostream &operator<<(std::ostream &os, const ModIntBase &a) {
        return os << a.val();
    }

    // three-way comparasion, all six relations
    friend constexpr std::strong_ordering operator<=>(const ModIntBase &lhs, const ModIntBase &rhs) {
        return lhs.val() <=> rhs.val();
    }
};

template<u32 P>
using ModInt = ModIntBase<u32, P>;
template<u64 P>
using ModInt64 = ModIntBase<u64, P>;

using Z = ModInt<MOD>;
using Matrix = vector<vector<Z>>;

/* Fixed-capacity, runtime-dimension matrix
   Storage is MAXN*MAXN no heap allocs. Flat 1D array
   Actual active dimension is n (<= MAXN)
   Multiplication loops only over [0..n) */
template<int MAXN, class ModInt>
struct MatDyn {
    using Z = ModInt;
    int n = 0;  // active dimension
    array<Z, MAXN*MAXN> a{}; // row-major storage
    MatDyn() = default;
    explicit MatDyn(int n_) : n(n_) {}

    // 2D to 1D Access through () operator
    inline Z& operator()(int i, int j) { return a[i * MAXN + j]; }
    inline const Z& operator()(int i, int j) const { return a[i * MAXN + j]; }

    static MatDyn identity(int n) {
        MatDyn m(n);
        for (int i = 0; i < n; i++) m(i, i) = 1;
        return m;
    }

    // Multiply: C = A * B. O(n^3)
    friend MatDyn operator*(const MatDyn& A, const MatDyn& B) {
        const int n = A.n;
        MatDyn C(n);
        for (int i = 0; i < n; i++) {
            for (int k = 0; k < n; k++) {
                Z aik = A(i, k);
                if (aik.val() == 0) continue;
                for (int j = 0; j < n; j++) {
                    C.a[i * MAXN + j] += aik * B.a[k * MAXN + j];
                }
            }
        }
        return C;
    }
};

template<int MAXN, class ModInt>
struct Monoid_MatDyn {
    using value_type = MatDyn<MAXN, ModInt>;
    using X = value_type;
    int n = 0;
    Monoid_MatDyn() = default;
    explicit Monoid_MatDyn(int n_) : n(n_) {}
    static X op(const X& x, const X& y) { return x * y; }
    X id() const { return X::identity(n); }
};

template<class Monoid>
struct DisjointSparseTable {
    using MX = Monoid;
    using X = typename MX::value_type;

    int n = 0;
    int LOG = 0; // number of levelsj
    vector<vector<X>> data; // data[k][i] either ending or stating at 'i', on level 'k'
    MX mx;

    explicit DisjointSparseTable(const vector<X>& a, MX mx_ = MX()) : mx(std::move(mx_)) { build(a); }
    template<class F>
    DisjointSparseTable(int n_, F f, MX mx_ = MX()) : mx(std::move(mx_)) { build(n_, f); }

    // Build from vector
    void build(const vector<X>& a) {
        build((int)a.size(), [&](int i) -> X { return a[i]; });
    }

    // Build from function f(i). O(n log n)
    template<class F>
    void build(int n_, F f) {
        n = n_;
        if (n == 0) { LOG = 0; data.clear(); return; }
        LOG = (n <= 1) ? 1 : 32 - __builtin_clz((unsigned)(n-1));
        data.assign(LOG, vector<X>(n));

        // level 0 holds the original array values
        for (int i = 0; i < n; i++) data[0][i] = f(i);
        // build higher levels. each level k uses block size B = 2^k
        for (int k = 1; k < LOG; k++) {
            const int B = 1 << k;
            data[k] = data[0]; // start with init values, then overwrite inside each block with prefix/suffix data
            for (int mid = B; mid < n; mid += B << 1) {
                int L = mid - B;
                int R = min(n, mid + B);
                // left half: compute suffixes ending at mid-1
                // data[k][i] = a[i] op ... op a[mid-1]
                for (int i = mid-2; i >= L; i--) {
                    data[k][i] = mx.op(data[0][i], data[k][i+1]);
                }

                // right half: compute prefixes starting at mid
                // data[k][i] = a[mid] op ... op a[i]
                if (mid < R) {
                    for (int i = mid + 1; i < R; i++) {
                        data[k][i] = mx.op(data[k][i-1], data[0][i]);
                    }
                }
            }
        }
    }

    // Query [l, r). O(1)
    X prod(int l, int r) const {
        if (l >= r) return mx.id();
        if (r - l == 1) return data[0][l];
        int a = l;
        int b = r - 1;
        // k = msb of (a^b): highest bit where they differ
        int k = 31 - __builtin_clz((unsigned)(a ^ b));
        return mx.op(data[k][a], data[k][b]);
    }
};

constexpr int MAXK = 105;
using Mat = MatDyn<MAXK, Z>;
using MX = Monoid_MatDyn<MAXK, Z>;

// Build character matrix T(ch)
template<int MAXK>
static Mat char_matrix(const string& pat, char ch) {
    int K = (int)pat.size();
    auto T = Mat::identity(K+1);
    for (int i = 0; i < K; i++) {
        if (pat[i] == ch) {
            T(i, i+1) += 1;
        }
    }
    return T;
}

// Example usage
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string S, P; cin >> S >> P;
    int q; cin >> q;

    int K = (int)P.size();

    // Build matrix array A[i] = T(S[i])
    MX mx(K+1);
    vector<Mat> A(S.size());
    for (int i = 0; i < (int)S.size(); i++) {
        A[i] = char_matrix<MAXK>(P, S[i]);
    }

    // DST over matrices
    DisjointSparseTable<MX> dst(A, mx);

    while (q--) {
        int l, r; cin >> l >> r;
        l--; // to 0-idx, [l, r)
        if (l >= r) {
            cout << (K == 0 ? 1 : 0);
            continue;
        }

        auto M = dst.prod(l, r); // product of matrices in order
        cout << M(0, K) << "\n"; // number of subsequenes in range [l, r) matching full template
    }

    return 0;
}
