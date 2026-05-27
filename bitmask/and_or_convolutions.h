#include "core.h"

/** Applies the OR zeta transform, or its inverse, in place. Time: O(n 2^n). */
template<class T>
void fwt_or(vector<T>& a, bool inverse=false) {
    int n = __builtin_ctz((int)a.size()); // size = 2^n
    for (int i = 0; i < n; i++) {
        for (int mask = 0; mask < (int)a.size(); mask++) {
            if (mask & (1<<i)) {
                if (!inverse) a[mask] += a[mask ^ (1<<i)];
                else a[mask] -= a[mask ^ (1<<i)];
            }
        }
    }
}

/** Applies the AND zeta transform, or its inverse, in place. Time: O(n 2^n). */
template<class T>
void fwt_and(vector<T>& a, bool inverse=false) {
    int n = __builtin_ctz((int)a.size()); // size = 2^n
    for (int i = 0; i < n; i++) {
        for (int mask = 0; mask < (int)a.size(); mask++) {
            if (mask & (1<<i)) {
                if (!inverse) a[mask] += a[mask | (1<<i)];
                else a[mask] -= a[mask | (1<<i)];
            }
        }
    }
}

/** Returns OR convolution of equal-size subset arrays. Time: O(n 2^n). Space: O(2^n). */
template<class T>
vector<T> or_convolution(vector<T> a, vector<T> b) {
    fwt_or(a, false); fwt_or(b, false);
    for (int i = 0; i < (int)a.size(); i++) a[i] *= b[i];
    fwt_or(a, true);
    return a;
}

/** Returns AND convolution of equal-size subset arrays. Time: O(n 2^n). Space: O(2^n). */
template<class T>
vector<T> and_convolution(vector<T> a, vector<T> b) {
    fwt_and(a, false); fwt_and(b, false);
    for (int i = 0; i < (int)a.size(); i++) a[i] *= b[i];
    fwt_and(a, true);
    return a;
}
