#include "../../header.h"

// Value-based version (Ji Driver style) - invariant: max1 and min1 always reflect real values
// Invariant chain: min1 < min2 <= max2 < max1
struct Beat {
    ll sum;
    ll max1, max2, min1, min2; // max1 > max2, min1 < min2
    ll maxc, minc;  // count of max1 and min1
    ll lazy_add;
};

struct BeatsSeg {
private:
    int n;
    vector<Beat> data;

    // Merge two kids into parent xd
    void pull(int v) {
        int left = v << 1;
        int right = v << 1 | 1;
        data[v].sum = data[left].sum + data[right].sum;

        // Max logic
        if (data[left].max1 == data[right].max1) {
            data[v].max1 = data[left].max1;
            data[v].maxc = data[left].maxc + data[right].maxc;
            data[v].max2 = max(data[left].max2, data[right].max2);
        } else if (data[left].max1 > data[right].max1) {
            data[v].max1 = data[left].max1;
            data[v].maxc = data[left].maxc;
            data[v].max2 = max(data[left].max2, data[right].max1);
        } else {
            data[v].max1 = data[right].max1;
            data[v].maxc = data[right].maxc;
            data[v].max2 = max(data[right].max2, data[left].max1);
        }

        // Min logic
        if (data[left].min1 == data[right].min1) {
            data[v].min1 = data[left].min1;
            data[v].minc = data[left].minc + data[right].minc;
            data[v].min2 = min(data[left].min2, data[right].min2);
        } else if (data[left].min1 < data[right].min1) {
            data[v].min1 = data[left].min1;
            data[v].minc = data[left].minc;
            data[v].min2 = min(data[left].min2, data[right].min1);
        } else {
            data[v].min1 = data[right].min1;
            data[v].minc = data[right].minc;
            data[v].min2 = min(data[left].min1, data[right].min2);
        }
    }

    // Apply generic addition
    void apply_add(int v, int l, int r, ll val) {
        if (val == 0) return;
        data[v].sum += (ll)(r-l+1) * val;
        data[v].max1 += val;
        if (data[v].max2 != -infty<ll>) data[v].max2 += val;
        data[v].min1 += val;
        if (data[v].min2 != infty<ll>) data[v].min2 += val;
        data[v].lazy_add += val;
    }

    // Apply min cap (chmin)
    void apply_min(int v, ll val) {
        if (data[v].max1 <= val) return; // break condition, nothing to do
        data[v].sum -= (data[v].max1 - val) * data[v].maxc;
        data[v].max1 = val;
        if (data[v].min1 > val) data[v].min1 = val; // sync min1 if needed
        if (data[v].min2 != infty<ll> && data[v].min2 > val) { // sync min2 if needed
            data[v].min2 = val;
        }
    }

    // Apply max cap (chmax)
    void apply_max(int v, ll val) {
        if (data[v].min1 >= val) return; // break condition, nothing to do
        data[v].sum += (val - data[v].min1) * data[v].minc;
        data[v].min1 = val;
        if (data[v].max1 < val) data[v].max1 = val; // sync max1 if needed
        if (data[v].max2 != -infty<ll> && data[v].max2 < val) { // sync max2 if needed
            data[v].max2 = val;
        }
    }

    // Push lazy tags down one level
    void push(int v, int l, int r) {
        if (l == r) return;
        int mid = (l+r)>>1;
        int L = v << 1;
        int R = v << 1 | 1;
        // 1) Push add
        if (data[v].lazy_add) {
            apply_add(L, l, mid, data[v].lazy_add);
            apply_add(R, mid+1, r, data[v].lazy_add);
            data[v].lazy_add = 0;
        }
        // 2) Push min (upper bound)
        // If parent's max is smaller than child's max, child must be constrained
        if (data[L].max1 > data[v].max1) apply_min(L, data[v].max1);
        if (data[R].max1 > data[v].max1) apply_min(R, data[v].max1);
        // 3) Push max (lower bound)
        // If parent's min is larger than child's min, child must be updated
        if (data[L].min1 < data[v].min1) apply_max(L, data[v].min1);
        if (data[R].min1 < data[v].min1) apply_max(R, data[v].min1);
    }

public:
    BeatsSeg(const vector<ll>& arr) {
        n = (int)arr.size();
        data.resize(4*n);
        build(arr, 1, 0, n-1);
    }

private:
    void build(const vector<ll>& arr, int v, int l, int r) {
        data[v].lazy_add = 0;
        if (l == r) {
            data[v].sum = arr[l];
            data[v].max1 = data[v].min1 = arr[l];
            data[v].max2 = -infty<ll>;
            data[v].min2 = infty<ll>;
            data[v].maxc = data[v].minc = 1;
            return;
        }
        int mid = (l+r)>>1;
        build(arr, v<<1, l, mid);
        build(arr, v<<1|1, mid+1, r);
        pull(v);
    }

    // a[i] = min(a[i], val)
    void update_chmin(int v, int tl, int tr, int l, int r, ll val) {
        if (l > tr || r < tl || data[v].max1 <= val) return; // exit condition
        if (tl >= l && tr <= r && data[v].max2 < val) { // tag
            apply_min(v, val);
            return;
        }
        push(v, tl, tr);
        int tm = (tl+tr)>>1;
        update_chmin(v<<1, tl, tm, l, r, val);
        update_chmin(v<<1|1, tm+1, tr, l, r, val);
        pull(v);
    }

    // a[i] = max(a[i], val)
    void update_chmax(int v, int tl, int tr, int l, int r, ll val) {
        if (l > tr || r < tl || data[v].min1 >= val) return; // exit condition
        if (tl >= l && tr <= r && data[v].min2 > val) { // tag
            apply_max(v, val);
            return;
        }
        push(v, tl, tr);
        int tm = (tl+tr)>>1;
        update_chmax(v<<1, tl, tm, l, r, val);
        update_chmax(v<<1|1, tm+1, tr, l, r, val);
        pull(v);
    }

    // a[i] = a[i] + val
    void update_add(int v, int tl, int tr, int l, int r, ll val) {
        if (l > tr || r < tl) return;
        if (tl >= l && tr <= r) {
            apply_add(v, tl, tr, val);
            return;
        }
        push(v, tl, tr);
        int tm = (tl+tr)>>1;
        update_add(v<<1, tl, tm, l, r, val);
        update_add(v<<1|1, tm+1, tr, l, r, val);
        pull(v);
    }

    ll query_sum(int v, int tl, int tr, int l, int r) {
        if (l > tr || r < tl) return 0;
        if (tl >= l && tr <= r) return data[v].sum;
        push(v, tl, tr);
        int tm = (tl+tr)>>1;
        return query_sum(v<<1, tl, tm, l, r) +
               query_sum(v<<1|1, tm+1, tr, l, r);
    }

public:
    void update_chmin(int l, int r, ll val) { update_chmin(1, 0, n-1, l, r, val); }
    void update_chmax(int l, int r, ll val) { update_chmax(1, 0, n-1, l, r, val); }
    void update_add(int l, int r, ll val) { update_add(1, 0, n-1, l, r, val); }
    ll query_sum(int l, int r) { return query_sum(1, 0, n-1, l, r); }
};
