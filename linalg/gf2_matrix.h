#pragma once
#include "../header.h"

template<int N>
struct GF2Matrix {
    array<bitset<N>, N> a{};

    bitset<N>& operator[](int i) { return a[i]; }
    const bitset<N>& operator[](int i) const { return a[i]; }

    static GF2Matrix id() {
        GF2Matrix I;
        for (int i = 0; i < N; i++) I[i][i] = 1;
        return I;
    }

    static GF2Matrix power(GF2Matrix base, ll exp) {
        GF2Matrix res = id();
        while (exp > 0) {
            if (exp & 1LL) res = res * base;
            base = base * base;
            exp >>= 1;
        }
        return res;
    }

    GF2Matrix operator^(ll exp) const {
        return power(*this, exp);
    }

    // matrix * vector
    bitset<N> mul_vec(const bitset<N>& v) const {
        bitset<N> res;
        for (int i = 0; i < N; i++) {
            // keep only positions where both row i and vector v have 1
            // count the number of 1s (mod 2)
            res[i] = ((a[i] & v).count() & 1ULL);
        }
        return res;
    }

    GF2Matrix operator*(const GF2Matrix& other) const {
        GF2Matrix res;
        array<bitset<N>, N> col{};
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (other[i][j]) col[j][i] = 1; // other[i][j] accesses row i, col j of other
                // col[j] is the j-th column of other
            }
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                res[i][j] = ((a[i] & col[j]).count() & 1ULL);
            }
        }
        return res;
    }

    int rank() const {
        auto tmp = a;
        int row = 0;

        for (int col = 0; col < N && row < N; col++) {
            int sel = -1;
            for (int i = row; i < N; i++) {
                if (tmp[i][col]) {
                    sel = i;
                    break;
                }
            }
            if (sel == -1) continue;

            swap(tmp[sel], tmp[row]);

            for (int i = 0; i < N; i++) {
                if (i != row && tmp[i][col]) tmp[i] ^= tmp[row];
            }
            row++;
        }
        return row;
    }

    // determinant in GF(2): 0 or 1
    int det() const {
        return rank() == N ? 1 : 0;
    }

    // TODO:
};
