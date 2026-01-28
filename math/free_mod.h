
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

