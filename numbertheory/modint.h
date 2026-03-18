#pragma once
#include "core.h"
#include "mod_arith.h"

namespace nt {


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
        x = mul_mod(x, rhs.val(), mod());
        return *this;
    }

    constexpr ModIntBase &operator+=(const ModIntBase &rhs) & {
        x += rhs.val();
        if (x >= mod()) x -= mod();
        return *this;
    }

    constexpr ModIntBase &operator-=(const ModIntBase &rhs) & {
        if (x < rhs.val()) x += mod();
        x -= rhs.val();
        return *this;
    }

    constexpr ModIntBase &operator/=(const ModIntBase &rhs) & {
        return *this *= rhs.inv();
    }

    constexpr ModIntBase pow(ll e) const {
        ModIntBase a = *this, res = 1;
        while (e > 0) {
            if (e & 1) res *= a;
            a *= a;
            e >>= 1;
        }
        return res;
    }

    // modular inverse (fermat's little theorem), mod() has to be prime
    constexpr ModIntBase inv() const {
        // return ModIntBase(pow_mod(this->val(), mod() - 2, mod()));
        return this->pow(mod()-2);
    }

    constexpr ModIntBase fast_inv() const {
        assert(x != 0);
        U t = x, res = 1;
        while (t != -1) {
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

    // three-way comparasion, all six relations
    friend constexpr std::strong_ordering operator<=>(const ModIntBase &lhs, const ModIntBase &rhs) {
        return lhs.val() <=> rhs.val();
    }
};

} // namespace nt
