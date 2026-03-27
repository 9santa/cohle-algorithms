#pragma once
#include "core.h"
#include "../strings/prefix_function.h"

/* string construction/counting while avoiding or requiring a pattern
   state: dp[pos][matchedPrefixLen][found]
   then dp is: dp[pos+1][go[st][c]][found || (go[st][c] == m)] += dp[pos][st][found]
   combines well with digit DP */

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
