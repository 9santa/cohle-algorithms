#pragma once
#include <bits/stdc++.h>
using namespace std;

using i128 = __int128;
using u64 = unsigned long long;
using i64 = long long;
using ll = long long;

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
