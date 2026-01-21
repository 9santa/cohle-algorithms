#include "../header.h"

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

class Comb {
public:
    int n;
    vector<Z> fac;
    vector<Z> invfac;

    Comb() : n(0), fac{1}, invfac{1} {}
    Comb(int _n) : Comb() {
        init(_n);
    }

    // dynamic initialization, can expand effectively
    void init(int m) {
        if (m <= n) return;
        fac.resize(m+1);
        invfac.resize(m+1);

        for (int i = n+1; i <= m; i++) {
            fac[i] = fac[i-1] * i;
        }
        invfac[m] = fac[m].inv();
        for (int i = m; i > n; i--) {
            invfac[i-1] = invfac[i] * i;
        }
        n = m;
    }

    Z get_fac(int m) {
        if (m > n) init(2 * m);
        return fac[m];
    }

    Z get_invfac(int m) {
        if (m > n) init(2 * m);
        return invfac[m];
    }

    Z binom(int n, int k) {
        if (n < k || k < 0) return 0;
        return get_fac(n) * get_invfac(k) * get_invfac(n - k);
    }

} comb;

template<typename T>
using Mat = vector<vector<T>>;

Mat<u32> mul_raw(const Mat<u32>& a, const Mat<u32>& b) {
    int n = (int)a.size();
    Mat<u32> c(n, vector<u32>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            u32 aik = a[i][k];
            if (!aik) continue;
            for (int j = 0; j < n; j++) {
                c[i][j] = (c[i][j] + (u64)aik * b[k][j]) % MOD;
            }
        }
    }
    return c;
}

Mat<Z> mul_modint(const Mat<Z>& a, const Mat<Z>& b) {
    int n = (int)a.size();
    Mat<Z> c(n, vector<Z>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            if (a[i][k].val() == 0) continue;
            Z aik = a[i][k];
            for (int j = 0; j < n; j++) {
                c[i][j] += aik * b[k][j];
            }
        }
    }
    return c;
}

template<class F>
double time_it(F&& f) {
    auto t0 = chrono::high_resolution_clock::now();
    f();
    auto t1 = chrono::high_resolution_clock::now();
    return chrono::duration<double>(t1 - t0).count();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n = 250;
    int reps = 200;

    mt19937 rng(123);
    uniform_int_distribution<int> dist(0, MOD-1);

    Mat<u32> Ar(n, vector<u32>(n, 0)), Br(n, vector<u32>(n, 0));
    Mat<Z> Az(n, vector<Z>(n, 0)), Bz(n, vector<Z>(n, 0));

    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) {
        u32 x = (u32)dist(rng);
        u32 y = (u32)dist(rng);
        Ar[i][j] = x; Br[i][j] = y;
        Az[i][j] = x; Bz[i][j] = y;
    }

    // warm-up
    volatile u32 sink = 0;
    {
        auto C = mul_raw(Ar, Br);
        sink ^= C[0][0];
    }
    {
        auto C = mul_modint(Az, Bz);
        sink ^= C[0][0].val();
    }

    double t_raw = time_it([&](){
        auto X = Ar;
        for (int r = 0; r < reps; r++) X = mul_raw(X, Br);
        sink ^= X[0][0];
    });

    double t_mod = time_it([&](){
        auto X = Az;
        for (int r = 0; r < reps; r++) X = mul_modint(X, Bz);
        sink ^= X[0][0].val();
    });

    cerr << "sink=" << sink << "\n";
    cout << "raw seconds:    " << t_raw << "\n";
    cout << "ModInt seconds: " << t_mod << "\n";
    cout << "ratio ModInt/raw: " << (t_mod / t_raw) << "x\n";
}

