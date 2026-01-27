
// 1D range min queries. O(1) time per query, O(N log N) preprocessing, O(N log N) mem
// immutable array, sparse table
template<typename T>
struct RMQ {
    int level(int x) { return 31-__builtin_clz(x); }    // max x: 2^x <= r-l, this is the leftmost set bit for segment (r-l)
    V<T> v; V<vi> jmp;  // jmp = sparse table with indices instead of values
    int cmb(int a, int b) { // get index of the min value
        return v[a]==v[b] ? std::min(a,b) : (v[a]<v[b] ? a : b);
    }
    void init(const V<T>& _v) {
        v = _v; jmp = {vi(sz(v))};
        std::iota(all(jmp[0]), 0);   // index of the min value of length 1 - is 'i' itself
        for (int j = 1; 1<<j <= sz(v); j++) {
            jmp.pb(vi(sz(v) - (1<<j)+1));
            F0R(i,sz(jmp[j])) jmp[j][i] = cmb(jmp[j-1][i], jmp[j-1][i+(1<<(j-1))]);   // split into two parts in the middle: 2^j = 2 * 2^(j-1)
        }
    }
    int index(int l, int r) {
        assert(l <= r); int d = level(r-l+1);
        return cmb(jmp[d][l],jmp[d][r-(1<<d)+1]);
    }
    T query(int l, int r) { return v[index(l, r)]; }
};

