
template <typename T> constexpr T binpow(T a, u64 b) {
    T res = 1;
    while (b > 0) {
        if (b & 1)
            res *= a;
        a *= a;
        b >>= 1;
    }
    return res;
}

// P has to be compile time known
template <u32 P> constexpr u32 mulMod(u32 a, u32 b) { return u64(a) * b % P; }

/* this is some tricky stuff, intentionally overflows u64
   but recovers the correct value using lond double approx */
template <u64 P> constexpr u64 mulMod(u64 a, u64 b) {
    u64 res = a * b - u64(1.L * a * b / P - 0.5L) * P;
    res %= P;
    return res;
}

constexpr i64 modpow(i64 a, i64 e, i64 p) {
    i64 res = 1 % p;
    a %= p;
    while (e > 0) {
        if (e & 1) res = (i128)res * a % p;
        a = (i128)a * a % p;
        e >>= 1;
    }
    return res;
}

// signed modulo fix
constexpr i64 safeMod(i64 x, i64 mod) {
    x %= mod;
    if (x < 0)
        x += mod;
    return x;
}

static i64 egcd_rec(i64 a, i64 b, i64 &x, i64 &y) {
    // returns g=gcd(a,b), and finds x,y such that ax+by=g
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    i64 x1, y1;
    i64 g = egcd_rec(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

// {g, x, y}: ax+by=g
template <typename T> static tuple<T, T, T> extgcd(T a, T b) {
    static_assert(is_same_v<T, int> || is_same_v<T, ll>);
    T x = 1, y = 0, x1 = 0, y1 = 0;
    while (b != 0) {
        T q = a / b;
        swap(a %= b, b);
        T X = x - q * x1, Y = y - q * y1;
        x = x1, y = y1, x1 = X, y1 = Y;
    }
    return {a, x, y};
}

static i64 modInv(i64 a, i64 mod) {
    // inverse exists if gcd(a, mod)=1
    auto [g, x, y] = extgcd(a, mod);
    if (g != 1) {
        return -1;
        // throw runtime_error("mod_inv: inverse does not exist");
    }
    return safeMod(x, mod);
}
