#pragma once
#include <bits/stdc++.h>
using namespace std;

using vi = vector<int>;
#define sz(x) int((x).size())

// Computes the prefix function (pi array) for string s.
// pi[i] = the length of the longest proper prefix of s[0..i]
//         which is also a suffix of s[0..i].
// O(|s|)
/** Computes the KMP prefix function. Time: O(|s|). Space: O(|s|). */
vector<int> prefix_function(const string& s) {
    int n = sz(s);
    vi pi(n);
    for (int i = 1; i < n; i++) {
        int j = pi[i-1];
        while (s[i] != s[j] && j != 0) j = pi[j-1];
        if (s[i] == s[j]) j++;
        pi[i] = j;
    }
    return pi;
}


/* string construction/counting while avoiding or requiring a pattern
   state: dp[pos][matchedPrefixLen][found]
   then dp is: dp[pos+1][go[st][c]][found || (go[st][c] == m)] += dp[pos][st][found]
   combines well with digit DP */

/** Builds KMP automaton transitions for pattern p. Time: O(|p| * alphabet). */
vector<vector<int>> buildAtomation(const string& p) {
    int m = sz(p);
    auto pi = prefix_function(p);
    vector<vector<int>> go(m+1, vector<int>(26, 0));

    for (int st = 0; st <= m; st++) {
        for (int c = 0; c < 26; c++) {
            if (st < m && ('A' + c) == p[st]) {
                go[st][c] = st + 1;
            } else if (st == 0) {
                go[st][c] = 0;
            } else {
                go[st][c] = go[pi[st-1]][c];
            }
        }
    }
    return go;
}
