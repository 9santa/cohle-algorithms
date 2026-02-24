#include "../header.h"

constexpr int MOD = 1e9+7;

// O(n * m * 2^m) time and memory
// N, M should be small. M is the smaller one (number of rows)
inline int domino_tiling(int n, int m) {
    // dp[i][j][p] -> number of ways to fill i first columns, j cells from above at i+1 column, with a broken profile p
    V<V<vi>> dp(n+1, V<vi>(m+1, vi(1<<m, 0)));
    dp[0][0][0] = 1;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int p = 0; p < (1 << m); p++) {
                if (p & (1<<j)) { // if new cell is occupied (bit is set)
                    dp[i][j+1][p ^ (1<<j)] = (dp[i][j+1][p ^ (1<<j)] + dp[i][j][p]) % MOD;
                } else {
                    // place horizontally to the right (always possible)
                    dp[i][j+1][p | (1<<j)] = (dp[i][j+1][p | (1<<j)] + dp[i][j][p]) % MOD;
                    if ((j < m-1) && ((p & (1<<(j+1))) == 0)) { // check if we can place vertically down
                        dp[i][j+2][p] = (dp[i][j+2][p] + dp[i][j][p]) % MOD;
                    }
                }
            }
        }
        // new column transition
        for (int p = 0; p < (1 << m); p++) {
            dp[i+1][0][p] = dp[i][m][p] % MOD;
        }
    }

    return dp[n][0][0] % MOD;
}

// With blocked cells
inline int domino_tiling_with_blocks(vector<string> g) {
    int R = sz(g);
    int C = sz(g[0]);

    // Do profile DP on smaller side: m = rows <= cols = n
    if (R > C) {
        vector<string> t(C, string(R, '.'));
        for (int r = 0; r < R; r++) {
            for (int c = 0; c < C; c++) {
                t[c][r] = g[r][c];
            }
        }
        g.swap(t);
        swap(R, C);
    }

    ll n = C; // columns
    int m = R; // rows (profile size)
    if ((n * 1LL * m) & 1LL) return 0;
    int S = 1 << m;

    // blocked[i] - mask of blocked cells in column i (bit = 1 if '#')
    vector<int> blocked(n, 0);
    for (int i = 0; i < (int)n; i++) {
        int mask = 0;
        for (int j = 0; j < m; j++) {
            if (g[j][i] == '#') mask |= (1 << j);
        }
        blocked[i] = mask;
    }

    vi start(S, 0); // dp before current column: dp[i][0][profile]
    start[0] = 1;

    for (int i = 0; i < (int)n; i++) {
        V<vi> dp(m+1, vi(S, 0));

        for (int p = 0; p < S; p++) {
            if ((p & blocked[i]) == 0) dp[0][p] = start[p];
        }

        for (int j = 0; j < m; j++) {
            for (int p = 0; p < S; p++) {
                int ways = dp[j][p];
                if (!ways) continue;

                bool is_block = (blocked[i] >> j) & 1;
                if (is_block) {
                    // if cell is blocked, it should be empty
                    if (p & (1 << j)) continue; // domino on blocked cell -> invalid
                    dp[j+1][p] = (dp[j+1][p] + ways) % MOD;
                    continue;
                }

                if (p & (1 << j)) {
                    // cell occupied (came from left) -> clear bit and go next
                    int np = p ^ (1 << j);
                    dp[j+1][np] = (dp[j+1][np] + ways) % MOD;
                } else {
                    // horizontally to the right
                    if (i + 1 < n && (((blocked[i+1] >> j) & 1) == 0)) {
                        int np = p | (1 << j);
                        dp[j+1][np] = (dp[j+1][np] + ways) % MOD;
                    }
                    // vertical down
                    if (j + 1 < m &&
                        (((p >> (j + 1)) & 1) == 0) &&
                        (((blocked[i] >> (j + 1)) & 1) == 0)) {
                            dp[j+2][p] = (dp[j+2][p] + ways) % MOD;
                    }
                }
            }
        }

        start = dp[m]; // move to the next column
    }

    return start[0];
}
