#pragma once
#include <bits/stdc++.h>
using namespace std;

using i128 = __int128;
using u64 = unsigned long long;
using i64 = long long;
using ll = long long;
using u32 = unsigned int;
#define sz(x) int((x).size())

namespace nt {

/**
 * Returns x modulo mod in [0, mod).
 * @param mod Positive modulus.
 * Time: O(1). Space: O(1).
 */
inline i64 safe_mod(i64 x, i64 mod) {
    x %= mod;
    if (x < 0) x += mod;
    return x;
}

/**
 * Returns a * b modulo mod without 64-bit overflow.
 * @param mod Positive modulus.
 * Time: O(1). Space: O(1).
 */
inline u64 mul_mod(u64 a, u64 b, u64 mod) {
    return (i128)a * b % mod;
}

/**
 * Returns a^e modulo mod.
 * @param mod Positive modulus.
 * Time: O(log e). Space: O(1).
 */
inline u64 pow_mod(u64 a, u64 e, u64 mod) {
    u64 res = 1 % mod;
    a %= mod;
    while (e) {
        if (e & 1) res = mul_mod(res, a, mod);
        a = mul_mod(a, a, mod);
        e >>= 1;
    }
    return res;
}

inline u64 inv_mod(u64 a, u64 MOD) {
    return pow_mod(a, MOD - 2, MOD);
}

} // namespace nt


namespace nt {


/**
 * Modular integer with compile-time modulus P.
 * @param U Unsigned storage type.
 * @param P Positive modulus.
 * Space: O(1) per value.
 */
template<std::unsigned_integral U, U P>
class ModIntBase {
private:
    U x;

public:
    constexpr ModIntBase() : x(0) {}

    /** Constructs from an unsigned value and reduces modulo P. Time: O(1). */
    template<std::unsigned_integral T>
    constexpr ModIntBase(T x_) : x(x_ % mod()) {} // reduce modulo P

    /** Constructs from a signed value and reduces into [0, P). Time: O(1). */
    template<std::signed_integral T>
    constexpr ModIntBase(T x_) {
        using S = std::make_signed_t<U>; // convert modulus to signed
        S v = x_ % S(mod()); // signed % signed
        if (v < 0) {
            v += mod();
        }
        x = v; // safe now
    }

    /** Returns the compile-time modulus. Time: O(1). */
    constexpr static U mod() {
        return P;
    }

    /** Returns the stored residue in [0, P). Time: O(1). */
    constexpr U val() const {
        return x;
    }

    /** Returns the additive inverse. Time: O(1). */
    constexpr ModIntBase operator-() const {
        ModIntBase res;
        res.x = (x == 0 ? 0 : mod() - x);
        return res;
    }

    /** Multiplies by rhs modulo P. Time: O(1). */
    constexpr ModIntBase &operator*=(const ModIntBase &rhs) & {
        x = mul_mod(x, rhs.val(), mod());
        return *this;
    }

    /** Adds rhs modulo P. Time: O(1). */
    constexpr ModIntBase &operator+=(const ModIntBase &rhs) & {
        x += rhs.val();
        if (x >= mod()) x -= mod();
        return *this;
    }

    /** Subtracts rhs modulo P. Time: O(1). */
    constexpr ModIntBase &operator-=(const ModIntBase &rhs) & {
        if (x < rhs.val()) x += mod();
        x -= rhs.val();
        return *this;
    }

    /** Divides by rhs using modular inverse. Time: O(log P). */
    constexpr ModIntBase &operator/=(const ModIntBase &rhs) & {
        return *this *= rhs.inv();
    }

    /** Returns this value raised to e. Time: O(log e). */
    constexpr ModIntBase pow(ll e) const {
        ModIntBase a = *this, res = 1;
        while (e > 0) {
            if (e & 1) res *= a;
            a *= a;
            e >>= 1;
        }
        return res;
    }

    /** Returns the modular inverse using Fermat's theorem. Requires prime modulus. Time: O(log P). */
    constexpr ModIntBase inv() const {
        // return ModIntBase(pow_mod(this->val(), mod() - 2, mod()));
        return this->pow(mod()-2);
    }

    /** Returns the modular inverse using Euclidean-style recurrence. Requires x != 0. Time: O(log P). */
    constexpr ModIntBase fast_inv() const {
        assert(x != 0);
        U t = x, res = 1;
        while (t != 1) {
            U z = mod() / t;
            res = (u64)res * (mod() - z) % mod();
            t = mod() - t * z;
        }
        return ModIntBase(res);
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

    friend std::istream &operator>>(std::istream &is, ModIntBase &a) {
        i64 i; is >> i; a = i; return is;
    }

    friend std::ostream &operator<<(std::ostream &os, const ModIntBase &a) {
        return os << a.val();
    }

    /** Compares residues by their stored value. Time: O(1). */
    friend constexpr std::strong_ordering operator<=>(const ModIntBase &lhs, const ModIntBase &rhs) {
        return lhs.val() <=> rhs.val();
    }
};

} // namespace nt


constexpr u32 MOD = 998244353;

namespace NTT {

using Z = nt::ModIntBase<u32, MOD>;
constexpr int G = 3; // primitive root modulo MOD

constexpr Z pow_z(Z a, ll e) {
    Z res = 1;
    while (e > 0) {
        if (e & 1) res *= a;
        a *= a;
        e >>= 1;
    }
    return res;
}

int base = 1; // how many levels or roots / rev are prepared
vector<Z> roots = {0, 1}; // roots[k + j] stores the j-th twiddle for stage block-size 2*k
vector<int> rev = {0, 1}; // rev stores bit-reversed indices

/** Ensures NTT roots/rev are prepared up to nbase. Time: O(2^nbase). */
void ensure_base(int nbase) {
    if (nbase <= base) return;
    rev.resize(1 << nbase);
    roots.resize(1 << nbase);
    for (int i = 0; i < (1 << nbase); i++) {
        rev[i] = (rev[i >> 1] >> 1) + ((i & 1) << (nbase - 1));
    }
    while (base < nbase) {
        Z z = pow_z(Z(G), (Z::mod() - 1) >> (base + 1));
        for (int i = 1 << (base - 1); i < (1 << base); i++) {
            roots[i << 1] = roots[i];
            roots[(i << 1) | 1] = roots[i] * z;
        }
        base++;
    }
}

/** In-place NTT. Time: O(n log n). */
void ntt(vector<Z>& a) {
    int n = sz(a);
    assert(n > 0 && (n & (n - 1)) == 0); // power of two
    int lg = __builtin_ctz(n);
    ensure_base(lg);
    int shift = base - lg;

    for (int i = 0; i < n; i++) {
        int j = rev[i] >> shift;
        if (i < j) swap(a[i], a[j]);
    }

    for (int k = 1; k < n; k <<= 1) {
        for (int i = 0; i < n; i += 2 * k) {
            for (int j = 0; j < k; j++) {
                Z w = a[i + j + k] * roots[j + k];
                a[i + j + k] = a[i + j] - w;
                a[i + j] = a[i + j] + w;
            }
        }
    }
}

/** In-place inverse NTT. Time: O(n log n). */
void intt(vector<Z>& a) {
    int n = sz(a);
    assert(n > 0 && (n & (n - 1)) == 0);
    if (n == 1) return;

    reverse(a.begin()+1, a.end());
    ntt(a);

    Z inv_n = Z(n).inv();
    for (Z& x : a) x *= inv_n;
}

}; // namespace NTT
