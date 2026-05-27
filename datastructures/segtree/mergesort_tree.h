#include <algorithm>
#include <functional>
#include <assert.h>
using namespace std;

/** Merge-sort tree for order-statistics style range counting queries. Space: O(n log n). */
template<typename T, class Compare = std::less<>>
struct MergeSortTree {
    int n;
    Compare cmp;
    vector<vector<T>> t;

    /** Builds sorted segment lists from a 0-indexed array. Time: O(n log n). */
    void build(const vector<int>& a, Compare _cmp = std::less<>()) {
        n = (int)a.size();
        t.resize(2 * n);
        cmp = _cmp;

        for (int i = 0; i < n; i++)
            t[n+i] = {a[i]};

        for (int i = n-1; i > 0; i--) {
            merge(t[i<<1].begin(), t[i<<1].end(),
                    t[i<<1|1].begin(), t[i<<1|1].end(),
                    back_inserter(t[i]), cmp);
        }
    }

    /** Counts elements <= x in [l, r). Time: O(log^2 n). */
    int count_less_or_equal(int l, int r, T x) const {
        assert(0 <= l && l <= r && r <= n);
        int res = 0;
        for (l += n, r += n; l < r; l>>=1, r>>=1) {
            if (l&1) {
                res += upper_bound(t[l].begin(), t[l].end(), x, cmp) - t[l].begin();
                l++;
            }
            if (r & 1) {
                --r;
                res += upper_bound(t[r].begin(), t[r].end(), x, cmp) - t[r].begin();
            }
        }
        return res;
    }

    /** Counts elements < x in [l, r). Time: O(log^2 n). */
    int count_less(int l, int r, T x) const {
        assert(0 <= l && l <= r && r <= n);
        int res = 0;
        for (l += n, r += n; l < r; l>>=1, r>>=1) {
            if (l & 1) {
                res += lower_bound(t[l].begin(), t[l].end(), x, cmp) - t[l].begin();
                l++;
            }
            if (r & 1) {
                --r;
                res += lower_bound(t[r].begin(), t[r].end(), x, cmp) - t[r].begin();
            }
        }
        return res;
    }

    /** Counts elements >= x in [l, r). Time: O(log^2 n). */
    int count_greater_or_equal(int l, int r, T x) const {
        assert(0 <= l && l <= r && r <= n);
        int res = 0;
        for (l += n, r += n; l < r; l>>=1, r>>=1) {
            if (l & 1) {
                res += t[l].end() - lower_bound(t[l].begin, t[l].end(), x, cmp);
                l++;
            }
            if (r & 1) {
                --r;
                res += t[r].end() - lower_bound(t[r].begin(), t[r].end(), x, cmp);
            }
        }
        return res;
    }

    /** Counts elements > x in [l, r). Time: O(log^2 n). */
    int count_greater(int l, int r, T x) const {
        assert(0 <= l && l <= r && r <= n);
        int res = 0;
        for (l += n, r += n; l < r; l>>=1, r>>=1) {
            if (l & 1) {
                res += t[l].end() - upper_bound(t[l].begin, t[l].end(), x, cmp);
                l++;
            }
            if (r & 1) {
                --r;
                res += t[r].end() - upper_bound(t[r].begin(), t[r].end(), x, cmp);
            }
        }
        return res;
    }

    /** Counts elements in [xl, xr) within index range [l, r). Time: O(log^2 n). */
    int count_within(int l, int r, T xl, T xr) const {
        assert(0 <= l && l <= r && r <= n);
        int res = 0;
        for (l += n, r += n; l < r; l>>=1, r>>=1) {
            if (l & 1) {
                res += lower_bound(t[l].begin(), t[l].end(), xr, cmp) - lower_bound(t[l].begin(), t[l].end(), xl, cmp);
                l++;
            }
            if (r & 1) {
                --r;
                res += lower_bound(t[r].begin(), t[r].end(), xr, cmp) - lower_bound(t[r].begin(), t[r].end(), xl, cmp);
            }
        }

    }

    /** Returns the sorted values from [l, r). Time: O(k log n), where k is the answer size. */
    vector<T> get_sorted_list(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);
        vector<T> res;
        for (l += n, r += n; l < r; l>>=1, r>>=1) {
            if (auto sz = (int)res.size(); l & 1) {
                res.insert(res.end(), t[l].begin(), t[l].end());
                inplace_merge(res.begin(), res.begin()+sz, res.end(), cmp);
                l++;
            }
            if (auto sz = (int)res.size(); r & 1) {
                --r;
                res.insert(res.begin(), t[r].begin(), t[r].end());
                inplace_merge(res.begin(), res.end()-sz, res.end(), cmp);
            }
        }
    }
};
