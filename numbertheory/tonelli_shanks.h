#include "core.h"
#include "legendre_jacobi_symbol.h"

namespace nt {

// Tonelli-Shanks: solve x^2 = a (mod p), p odd prime
// returns one root x, or -1 if no solution
inline ll tonelli_shanks(ll a, ll p) {
    a %= p;
    if (a < 0) a += p;
    if (a == 0) return 0;
    if (p == 2) return a;
    if (legendre_symbol(a, p) != 1) return -1;

    if (p % 4 == 3) {
        return pow_mod(a, (p + 1) / 4, p);
    }

    ll q = p - 1;
    int s = 0;
    while ((q&1) == 0) {
        q >>= 1;
        s++;
    }

    ll z = 2;
    while (legendre_symbol(z, p) != -1) z++;

    ll c = pow_mod(z, q, p);
    ll x = pow_mod(a, (q + 1) / 2, p);
    ll t = pow_mod(a, q, p);
    int m = s;

    while (t != 1) {
        int i = 0;
        ll cur = t;
        while (cur != 1) {
            cur = (ll)((i128)cur * cur % p);
            i++;
        }

        ll b = pow_mod(c, 1LL << (m - i - 1), p);
        x = (ll)((i128)x * b % p);
        t = (ll)((i128)t * b % p * b % p);
        c = (ll)((i128)b * b % p);
        m = i;
    }

    return x; // second root is p - x;
}

} // namespace nt
