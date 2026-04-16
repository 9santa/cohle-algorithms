#pragma once
#include <bits/stdc++.h>
#include <iterator>
using namespace std;

// Works for string, vector<>, or any container with size() and operator[]
// Time: O(n * m)
// Memory: O(min(n, m))
template<class Seq>
int levenshtein_distance(const Seq& a, const Seq& b) {
    int n = (int)a.size();
    int m = (int)b.size();

    // keep dp array on the smaller sequence
    if (m > n) return levenshtein_distance(b, a);

    vector<int> dp(m+1);
    iota(dp.begin(), dp.end(), 0); // dp[j] = j

    for (int i = 1; i <= n; i++) {
        int prev = dp[0]; // dp[i-1][j-1]
        dp[0] = i; // dp[i][0] -> converting first i chars of a to empty string takes i deletions

        for (int j = 1; j <= m; j++) {
            int temp = dp[j]; // old dp[i-1][j]

            if (a[i-1] == b[j-1]) dp[j] = prev;
            else {
                dp[j] = 1 + min({
                    prev,       // replace
                    dp[j],      // delete
                    dp[j-1]     // insert
                    });
            }
            prev = temp;
        }
    }

    return dp[m];
}

// Version for Iterators instead of indexable containers
template<class It1, class It2>
int levenshtein_distance(It1 first1, It1 last1, It2 first2, It2 last2) {
    using T1 = typename iterator_traits<It1>::value_type;
    using T2 = typename iterator_traits<It2>::value_type;

    vector<T1> a(first1, last1);
    vector<T2> b(first2, last2);
    int n = (int)a.size();
    int m = (int)b.size();

    if (m > n) {
        vector<int> dp(n+1);
        iota(dp.begin(), dp.end(), 0);
        for (int i = 1; i <= m; i++) {
            int prev = dp[0];
            dp[0] = i;
            for (int j = 1; j <= n; j++) {
                int temp = dp[j];
                if (b[i-1] == a[i-1]) dp[j] = prev;
                else dp[j] = 1 + min({prev, dp[j], dp[j-1]});
                prev = temp;
            }
        }
        return dp[n];
    }

    vector<int> dp(m + 1);
    iota(dp.begin(), dp.end(), 0);
    for (int i = 1; i <= n; i++) {
        int prev = dp[0];
        dp[0] = i;
        for (int j = 1; j <= m; j++) {
            int temp = dp[j];
            if (a[i-1] == b[i-1]) dp[j] = prev;
            else dp[j] = 1 + min({prev, dp[j], dp[j-1]});
            prev = temp;
        }
    }
    return dp[m];
}


