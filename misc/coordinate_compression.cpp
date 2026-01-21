#include <algorithm>
#include <bits/stdc++.h>
#include <ctime>
#include <functional>

#define pb push_back
#define eb emplace_back
#define all(a) (a).begin(), (a).end()

using namespace std;

using i64 = long long;
using u64 = unsigned long long;
using u32 = unsigned int;
using u128 = unsigned __int128;
using i128 = __int128;
using ld = long double;
using pii = pair<int, int>;

// Coordinate Compression
vector<int> coordinate_compression(vector<int>const& arr) {
    int n = (int)arr.size();

    // 1 Step: Collect and Sort unique values
    vector<int> sorted = arr;
    sort(sorted.begin(), sorted.end());
    sorted.erase(unique(sorted.begin(), sorted.end()), sorted.end());

    // 2 Step: Mapping function
    function<int(int, vector<int> const&)> get_compressed_index = [&](int value, vector<int>const& sorted){
        return lower_bound(sorted.begin(), sorted.end(), value) - sorted.begin() + 1;
    };

    // 3 Step: Apply compression
    vector<int> compressed_indices(n);
    for(int i = 0; i < n; i++) {
        compressed_indices[i] = get_compressed_index(arr[i], sorted);
    }

    return compressed_indices;
}

/*
Original array:   1   3   5   4   7
Compressed ranks: 1   2   4   3   5
*/
