#include <vector>

using std::vector;

// Prefix Sums and Suffix Sums
class PrefixSuffixSums {
public:
    // Build Prefix Sum array
    static vector<int> build_prefix_sum(const vector<int>& arr) {
        int n = (int)arr.size();
        vector<int> prefix_sum(n+1, 0);
        // prefix_sum.assign(n+1);

        for(int i = 0; i < n; i++) {
            prefix_sum[i+1] = prefix_sum[i] + arr[i];
        }

        return prefix_sum;
    }

    // Build Suffix sum array
    static vector<int> build_suffix_sum(const vector<int>& arr) {
        int n = (int)arr.size();
        vector<int> suffix_sum(n, 0);
        // suffix_sum.assign(n);

        for(int i = n-1; i >= 0; i--) {
            if(i == n-1) suffix_sum[i] = arr[i];
            else suffix_sum[i] = suffix_sum[i+1] + arr[i];
        }

        return suffix_sum;
    }

    // Respond to Segment Sum Query
    static int query_sum(const vector<int>& prefix_sum, int l, int r) {
        return prefix_sum[r+1] - prefix_sum[l]; 
    }

    // 2D Prefix Sum (Integral Image)
    static vector<vector<int>> build_2D_prefix_sum(const vector<vector<int>>& arr) {
        int n = (int)arr.size();
        int m = (int)arr[0].size();
        vector<vector<int>> prefix_sum_2d(n+1, vector<int>(m+1, 0));
        // prefix_sum_2d.assign(n+1, vector<int>(m+1, 0));

        for(int i = 1; i <= n; i++) {
            for(int j = 1; j <= m; j++) {
                prefix_sum_2d[i][j] = arr[i-1][j-1] +
                                      prefix_sum_2d[i-1][j] +
                                      prefix_sum_2d[i][j-1] -
                                      prefix_sum_2d[i-1][j-1];
            }
        }

        return prefix_sum_2d;
    }

    // Sub-Matrix Sum
    static int query_sum_2d(const vector<vector<int>>& prefix_sum_2d, int x1, int y1, int x2, int y2) {
        return prefix_sum_2d[x2+1][y2+1] -
               prefix_sum_2d[x1][y2+1] -
               prefix_sum_2d[x2+1][y1] +
               prefix_sum_2d[x1][y1];
    }

};

// Prefix and Suffix Min & Max
class PrefixSuffixMinMax {
public:
    vector<int> suf_max;
    vector<int> suf_min;

    vector<int> suf_max_idx;
    vector<int> suf_min_idx;

    vector<int> pre_max;
    vector<int> pre_min;

    vector<int> pre_max_idx;
    vector<int> pre_min_idx;

    void SuffixMax(const vector<int>& a) {
        int n = (int)a.size();

        suf_max.reserve(n);
        suf_max_idx.reserve(n);

        suf_max[n-1] = a[n-1];
        suf_max_idx[n-1] = n-1;

        for(int i = n-1; i >= 0; i--) {
            if(a[i] > suf_max[i+1]) {
                suf_max[i] = a[i];
                suf_max_idx[i] = i;
            } else {
                suf_max[i] = suf_max[i+1];
                suf_max_idx[i] = i+1;
            }
        }
    }

    void SuffixMin(const vector<int>& a) {
        int n = (int)a.size();

        suf_min.reserve(n);
        suf_min_idx.reserve(n);

        suf_min[n-1] = a[n-1];
        suf_min_idx[n-1] = n-1;

        for(int i = n-2; i >= 0; i--) {
            if(a[i] < suf_min[i+1]) {
                suf_min[i] = a[i];
                suf_min_idx[i] = i;
            } else {
                suf_min[i] = suf_min[i+1];
                suf_min_idx[i] = suf_min_idx[i+1];
            }
        }
    }

    void PrefixMax(const vector<int>& a) {
        int n = (int)a.size();

        pre_max[0] = a[0];
        pre_max_idx[0] = 0;

        for(int i = 1; i < n; i++) {
            if(a[i] > pre_max[i-1]) {
                pre_max[i] = a[i];
                pre_max_idx[i] = i;
            } else {
                pre_max[i] = pre_max[i-1];
                pre_max_idx[i] = pre_max_idx[i-1];
            }
        }
    }

    void PrefixMin(const vector<int>& a) {
        int n = (int)a.size();

        pre_min[0] = a[0];
        pre_min_idx[0] = 0;

        for(int i = 1; i < n; i++) {
            if(a[i] < pre_max[i-1]) {
                pre_max[i] = a[i];
                pre_max_idx[i] = i;
            } else {
                pre_max[i] = pre_max[i-1];
                pre_max_idx[i] = pre_max_idx[i-1];
            }
        }
    }
};
