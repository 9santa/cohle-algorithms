#include "../header.h"

/*
Hamilton Path in a directed or undirected path using subset DP
dp[mask][v] = reachable (exists a path visiting exactly vertices in 'mask' nad ending at 'v')

Time: O(2^n * n^2). Works only for n <= 25
Memory: O(2^n * n)
*/

static vector<int> hamilton_path(const vector<vector<int>>& matrix, bool isDirected) {
    int n = (int)matrix.size();
    const int FULL = (1 << n) - 1;
    vector<vector<char>> dp(1<<n, vector<char>(n, 0));
    vector<vector<int>> parent(1<<n, vector<int>(n, -1));

    for (int v = 0; v < n; v++) dp[1<<v][v] = 1;

    for (int mask = 0; mask <= FULL; mask++) {
        for (int v = 0; v < n; v++) {
            if (!(mask & (1 << v))) continue; // v not in mask
            if (!dp[mask][v]) continue;

            for (int nxt = 0; nxt < n; nxt++) {
                if (mask & (1 << nxt)) continue; // already visited
                if (!matrix[v][nxt]) continue; // no edge v -> next

                int nmask = mask | (1 << nxt);
                if (!dp[nmask][nxt]) {
                    dp[nmask][nxt] = 1;
                    parent[nmask][nxt] = v;
                }
            }
        }
    }

    // find any end vertex for a full-mask path
    int endv = -1;
    for (int v = 0; v < n; v++) {
        if (dp[FULL][v]) { endv = v; break; }
    }

    if (endv == -1) {
        cout << "No\n";
        return {};
    }

    // reconstruct path
    vector<int> path;
    int mask = FULL;
    int v = endv;
    while (v != -1) {
        path.push_back(v);
        int pv = parent[mask][v];
        mask &= ~(1 << v); // set 0 at bit 'v'
        v = pv;
    }
    reverse(all(path));

    return path;
}
