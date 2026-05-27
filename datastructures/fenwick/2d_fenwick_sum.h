#include <vector>
using namespace std;

/** 2D Fenwick tree for point additions and rectangle sum queries. Space: O(nm). */
template<typename T>
struct Fenwick2D {
    int n, m;
    vector<vector<T>> fw;

    Fenwick2D() = default;

    /** Creates an empty n by m tree. */
    Fenwick2D(int _n, int _m) : n(_n), m(_m), fw(n+1, vector<T>(m+1)) {}

    /** Adds val to point (r, c). Time: O(log n log m). */
    void update(int r, int c, T val) {
        r++, c++;
        for (; r <= n; r += r & -r) {
            for (int i = c; i <= m; i += i & -i) { fw[r][i] += val; }
        }
    }

    /** Returns the sum over rows [0, r] and columns [0, c]. Time: O(log n log m). */
    T rect_sum(int r, int c) const {
        r++, c++;
        T sum = 0;
        for (; r > 0; r -= r & -r) {
            for (int i = c; i > 0; i -= i & -i) { sum += fw[r][i]; }
        }
        return sum;
    }

    /** Returns the sum over rectangle [r1, r2] x [c1, c2]. Time: O(log n log m). */
    T rect_sum(int r1, int c1, int r2, int c2) const {
        return rect_sum(r2, c2) - rect_sum(r2, c1 - 1) - rect_sum(r1 - 1, c2) +
               rect_sum(r1 - 1, c1 - 1);
    }
};
