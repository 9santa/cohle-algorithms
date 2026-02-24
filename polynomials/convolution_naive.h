
inline vector<int> convolution_naive(const vector<int>& P, const vector<int>& Q) {
    size_t n = P.size() + Q.size() - 1;
    vector<int> R(n, 0);
    for (int i = 0; i < sz(P); i++) {
        for (int j = 0; j < sz(Q); j++) {
            R[i + j] += P[i] * Q[j];
        }
    }
    return R;
}
