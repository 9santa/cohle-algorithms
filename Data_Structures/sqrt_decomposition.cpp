#include "../header.h"

struct SqrtDecomp {
    int n, len, blocks;
    vi a;     // copy of the input array
    vi b;     // block sums
    vi mn;    // block min
    vi mx;    // block max
    vi zeros; // count of zeros in block

    // preprocessing O(n)
    void build(const vi& arr) {
        a = arr;
        n = sz(a);
        len = (int)sqrt(n + .0) + 1; // size of the block
        blocks = (n + len - 1) / len;   // number of blocks
        b.resize(blocks);
        mn.assign(blocks, INT_MAX);
        mx.assign(blocks, INT_MIN);
        zeros.assign(blocks, 0);

        for (int i = 0; i < n; i++) {
            int blk = i / len;
            b[blk] += a[i];
            mn[blk] = min(mn[blk], a[i]);
            mx[blk] = max(mx[blk], a[i]);
            zeros[blk] += (a[i] == 0);
        }
    }

    // answering queries (bad) O(sqrt(n))
    int bad_sum(int l, int r) {
        int res = 0;
        for (int i = l; i <= r;) {
            if (i % len == 0 && i + len - 1 <= r) {
                res += b[i / len];
                i += len;
            } else {
                res += a[i];
                i++;
            }
        }
        return res;
    }

    // answering queries (good) O(sqrt(n))
    int sum(int l, int r) {
        int res = 0;
        int cl = l / len, cr = r / len;
        if (cl == cr) {
            for (int i = l; i <= r; i++) {
                res += a[i];
            }
        } else {
            for (int i = l, end=(cl+1)*len-1; i <= end; i++) res += a[i];
            for (int i = cl+1; i <= cr-1; i++) res += b[i];
            for (int i = cr*len; i <= r; i++) res += a[i];
        }
        return res;
    }

    // point update
    void update(int i, int val) {
        int blk = i / len;
        b[blk] += val - a[i];
        if (a[i] == 0) zeros[blk]--;
        if (val == 0) zeros[blk]++;

        mn[blk] = INT_MAX;
        mx[blk] = INT_MIN;
        for (int j = blk*len; j < min(n, (blk+1) * len); j++) {
            mn[blk] = min(mn[blk], a[j]);
            mx[blk] = max(mx[blk], a[j]);
        }

        a[i] = val;
    }
};

struct LazySqrt {
    int n, len, blocks;
    vl a;   // copy of the input array
    vl add; // lazy add per block

    void build(const vector<ll>& arr) {
        a = arr;
        n = sz(a);
        len = (int)sqrt(n) + 1;
        blocks = (n + len - 1) / len;
        add.assign(blocks, 0);
    }

    void range_add(int l, int r, ll delta) {
        int bl = l / len;
        int br = r / len;

        if (bl == br) {
            for (int i = l; i <= r; i++)
                a[i] += delta;
            return;
        }
        // left tail
        int endleft = (bl+1) * len - 1;
        for (int i = l; i <= min(endleft, n-1); i++) {
            a[i] += delta;
        }
        // right tail
        int startright = br * len;
        for (int i = startright; i <= min(r, n-1); i++) {
            a[i] += delta;
        }
        // full blocks between
        for (int b = bl+1; b <= br-1; b++) {
            add[b] += delta;
        }
    }

    ll point_query(int i) const {
        int b = i / len;
        return a[i] + add[b];
    }
};
