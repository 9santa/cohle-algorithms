
// 'a' should be an 2^n all subsets array
template<class T>
void fwt_xor(vector<T>& a, bool inv=false) {
    int n = (int)a.size();
    assert(!(n & (n-1))); // check if n is power of 2
    for (int len = 1; len < n; len <<= 1) {
        for (int i = 0; i < n; i = (i+len) << 1) {
            for (int j = 0; j < len; j++) {
                ll u = a[i + j];
                ll v = a[i + j + len];
                a[i + j] = u + v;
                a[i + j + len] = u - v;
            }
        }
    }
    if (inv) {
        for (int i = 0; i < n; i++)
            a[i] /= n;
    }
}
