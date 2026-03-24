#include "mod_arith.h"

namespace nt {

// Legendre symbol (a | p), p odd prime
inline int legendre_symbol(ll a, ll p) {
    a %= p;
    if (a < 0) a += p;
    if (a == 0) return 0;
    ll t = pow_mod(a, (p-1)/2, p);
    if (t == 1) return 1;
    if (t == p - 1) return -1;
    return 0; // should not happen for prime p
}

// Jacobi symbol (a | n), n odd positive
inline int jacobi_symbol(ll a, ll n) {
    if (n <= 0 || (n % 2 == 0)) return 0;
    a %= n;
    if (a < 0) a += n;

    int ans = 1;
    while (a != 0) {
        while ((a & 1) == 0) {
            a >>= 1;
            int r = n % 8;
            if (r == 3 || r == 5) ans = -ans;
        }
        swap(a, n);
        if ((a % 4 == 3) && (n % 4 == 3)) ans = -ans;
        a %= n;
    }
    return (n == 1 ? ans : 0);
}

} // namespace nt
