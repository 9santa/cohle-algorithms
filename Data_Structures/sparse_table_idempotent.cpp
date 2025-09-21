#include <functional>
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>


// Templated version for different idempotent operations
template<typename T, typename Compare = std::less<T>>
class GenericSparseTable {
private:
    std::vector<std::vector<T>> table;
    int n;
    int padded_n;
    Compare comp;
    T identity;

    int next_power_of_2(int x) {
        if (x <= 0) return 1;
        if (x == 1) return 2;
        return 1 << (32 - __builtin_clz(x - 1));
    }

    int log2_floor(int x) {
        return 31 - __builtin_clz(x);
    }

public:
    GenericSparseTable(std::vector<T> const& arr, Compare cmp = Compare(), T id = T())
                        : n((int)arr.size()), comp(cmp), identity(id) {

        if (n == 0) return;
    
        padded_n = next_power_of_2(n);
        int k_max = log2_floor(padded_n) + 1;
        table.resize(k_max, std::vector<T>(padded_n, identity));

        // Initialize first level
        for(int i = 0; i < n; i++) {
            table[0][i] = arr[i];
        }

        for(int i = n; i < padded_n; i++) {
            table[0][i] = identity;
        }

        // Build the table
        for(int j = 1; j < k_max; j++) {
            int step = 1 << (j - 1);
            for(int i = 0; i + (1 << j) <= padded_n; i++) {
                table[j][i] = comp(table[j-1][i], table[j-1][i+step]) ? 
                                table[j-1][i] : table[j-1][i+step];
            }
        }
    }

    // Query [l, r] (0-indexed, inclusive)
    T query(int l, int r) {
        if (l < 0) l = 0;
        if (r >= n) r = n - 1;
        if (l > r) return identity;

        int length = r - l + 1;
        int k = log2_floor(length);

        return comp(table[k][l], table[k][r - (1 << k) + 1]) ? 
               table[k][l] : table[k][r - (1 << k) + 1];
    }
};

// usage example
int main(void) {

    std::vector<int> arr = {1, 3, 2, 7, 9, 11, 5, 8, 4, 6};

    GenericSparseTable<int, std::greater<int>> st_max(arr, std::greater<int>(), INT_MIN);
    std::cout << "\nMaximum queries:\n";
    std::cout << "query(2, 7) = " << st_max.query(2, 7) << " (expected: 11)\n";
    std::cout << "query(0, 4) = " << st_max.query(0, 4) << " (expected: 9)\n";


    return 0;
}
