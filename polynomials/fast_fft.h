#include "../header.h"

namespace CFFT {
using real = double;

struct cmpl {
    real x, y;
    cmpl(real _x=0, real _y=0) : x(_x), y(_y) {}
    inline cmpl operator + (const cmpl& o) const { return cmpl(x+o.x, y+o.y); }
    inline cmpl operator - (const cmpl& o) const { return cmpl(x-o.x, y-o.y); }
    inline cmpl operator * (const cmpl& o) const { return cmpl(x*o.x - y*o.y, x*o.y + y*o.x); }
    inline cmpl operator * (real k)        const { return cmpl(x*k, y*k); }
    inline cmpl conj() const { return cmpl(x, -y); }
};

const real PI = acosl(-1.0);

int base = 1;
vector<cmpl> rts = {cmpl(0, 0), cmpl(1, 0)};
vector<int> rev = {0, 1};

void ensure_base(int nbase) {
    if (nbase <= base) return;
    rev.resize(1 << nbase);
    rts.resize(1 << nbase);
    for (int i = 0; i < (1 << nbase); i++) {
        rev[i] = (rev[i >> 1] >> 1) + ((i & 1) << (nbase - 1));
    }
    while (base < nbase) {
        real angle = PI * 2.0 / (1 << (base + 1));
        for (int i = 1 << (base - 1); i < (1 << base); i++) {
            rts[i << 1] = rts[i];
            real angle_i = angle * (2 * i + 1 - (1 << base));
            rts[(i << 1) + 1] = cmpl(cos(angle_i), sin(angle_i));
        }
        ++base;
    }
}

void fft(vector<cmpl>& a) {
    int n = sz(a);
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
                cmpl w = a[i + j + k] * rts[j + k];
                a[i + j + k] = a[i + j] - w;
                a[i + j] = a[i + j] + w;
            }
        }
    }
}

void ifft(vector<cmpl>& a) {
    int n = sz(a);
    for (auto& z : a) z = z.conj();
    fft(a);
    for (auto& z : a) { z = z.conj(); z.x /= n; z.y /= n; }
}

} // namespace CFFT
