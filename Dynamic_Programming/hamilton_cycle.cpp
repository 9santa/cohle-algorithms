#include "../header.h"

/*
Hamilton Cycle using subset DP
Fix start = 0
dp[mask][v] = reachable: exists a path starting at 0, visiting exactly 'mask' subset, ending at v
parent[mask][v] = prev vertex on that path

Time: O(2^n * n^2). For n ~<= 25
Memoery: O(2^n * n)
*/

static vector<int> hamilton_cycle(const vector<vector<int>>& matrix, bool isDirected) {
    int n = (int)matrix.size();
    if (n == 0) return {};
    if (n == 1) return matrix[0][0] ? vector<int>{0,0} : vector<int> {};

    const int START = 0;
    const int FULL = (1 << n) - 1;

    vector<vector<char>> dp(1<<n, vector<char>(n, 0));
    vector<vector<int>> parent(1<<n, vector<int>(n, -1));

    // base: path that starts at START and ends at START with only START visited
    dp[1<<START][START] = 1;

    for (int mask = 0; mask <= FULL; mask++) {
        if (!(mask & (1 << START))) continue; // only consider masks that include START (optimization)

        for (int v = 0; v < n; v++) {
            if (!(mask & (1 << v))) continue;
            if (!dp[mask][v]) continue;

            for (int nxt = 0; nxt < n; nxt++) {
                if (mask & (1 << nxt)) continue; // already visited
                if (!matrix[mask][v]) continue; // no edge v -> nxt

                int nmask = mask | (1 << nxt);
                if (!dp[nmask][nxt]) {
                    dp[nmask][nxt] = 1;
                    parent[nmask][nxt] = v;
                }
            }
        }
    }

    // To make a cycle, end at some v != START with FULL visited and edge v -> START exists
    int endv = -1;
    for (int v = 0; v < n; v++) {
        if (v == START) continue;
        if (dp[FULL][v] && matrix[v][START]) {
            endv = v;
            break;
        }
    }

    if (endv == -1) { return {}; } // no hamilton cycle

    vector<int> path;
    int mask = FULL;
    int v = endv;
    while (v != -1) {
        path.push_back(v);
        int pv = parent[mask][v];
        mask &= ~(1 << v);  // set 0 at 'v'
        v = pv;
    }
    reverse(all(path));
    path.push_back(START); // close cycle

    return path; // path is like: 0 a b c ... endv 0
}
