#include <iostream>
#include <climits>
#include <vector>


// Sparse Table for Associative operations
class SparseTableAssociative {
private:
    std::vector<std::vector<int>> table;
    int n;          // original array size
    int padded_n;   // padded size (closest power of 2)
    int k_max;

    // calculate next power of 2 >= x
    int next_power_of_2(int x) {
        if (x <= 0) return 1;
        if (x == 1) return 2;
        return 1 << (32 - __builtin_clz(x - 1));
    }

    // calculate floor(log2(x))
    int log2_floor(int x) {
        return 31 - __builtin_clz(x);
    }

public:
    SparseTableAssociative(std::vector<int> const& arr) : n((int)arr.size()) {
        if (n == 0) return;

        // pad to closest power of 2
        padded_n = next_power_of_2(n);

        k_max = log2_floor(padded_n) + 1;
        table.resize(k_max, std::vector<int>(padded_n, INT_MAX));



        // Level 0: intervals of length 1 (2^0)
        for(int i = 0; i < n; i++) {
            table[0][i] = arr[i];
        }

        // Pad the rest with INT_MAX
        for(int i = n; i < padded_n; i++) {
            table[0][i] = INT_MAX;  // pad with infinity (for not messing up the minimums)
        }

        // Build table for intervals of length 2^j
        for(int j = 1; j < k_max; j++) {
            int step = 1 << (j - 1);    // half the current interval length
            for(int i = 0; i + (1 << j) <= padded_n; i++) {
                table[j][i] = table[j-1][i] + table[j-1][i + step];
            }
        }
    }

        // Query for sum in range [l, r] (0-indexed, inclusive)
        int query(int l, int r) {
            if (l < 0) l = 0;
            if (r >= n) r = n - 1;
            if (l > r) return INT_MAX;

            long long sum = 0;
            for(int i = k_max; i >= 0; i--) {
                int length = r - l + 1;
                if((1 << i) <= length) {
                    sum += table[i][l];
                    l += 1 << i;
                }
            }

            return sum;
        }
};

// testing examples
int main(void) {
    std::vector<int> arr = {1, 3, 2, 7, 9, 11, 5, 8, 4, 6};

    SparseTableAssociative st(arr);

    std::cout << st.query(3, 8);

    return 0;
}

