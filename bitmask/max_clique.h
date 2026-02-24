#include "../header.h"

/*
Maximum Clique via Meet-in-the-Middle
- Split vertices into A (n1) and B (n2)
- Enumerate all subsets of B:
    isCliqueB[mask] computed via dp on removing LSP
    best[mask] = max clique size among submasks of mask (subset DP)
- Enumerate all subsets of A that are cliques:
    for each clique SA, compute compatible vertices in B that connect to all in SA
    answer = max(|SA| + best[compatMask])

- n up to ~60
Time: O(2 ^ (n/2) * poly(n))
Memory: O(2 ^ (n/2))
*/

static void solve() {
    int n, m; cin >> n >> m;
    vector<u64> adj(n, 0); // adj[i] stores mask of all outgoing edges
    for (int i = 0; i < m; i++) {
        int u, v; cin >> u >> v;
        if (u == v) continue;
        adj[u] |= (1ull << v);
        adj[v] |= (1ull << u);
    }

    int n1 = n / 2;
    int n2 = n - n1;

    // Map A = [0..n1-1], B = [n1..n-1]
    // Build adjacency masks within A, B and cross A->B in B-indexed masks
    vector<u32> adjA(n1, 0);
    vector<u32> adjB(n2, 0);
    vector<u32> adjAB(n1, 0);

    for (int a = 0; a < n1; a++) {
        u64 na = adj[a];
        // within A
        u32 maskA = 0;
        for (int x = 0; x < n1; x++) {
            if (na & (1ull << x)) maskA |= (1u << x);
        }
        adjA[a] = maskA;

        // cross to B
        u32 maskB = 0;
        for (int x = 0; x < n2; x++) {
            int v = n1 + x;
            if (na & (1ull << v)) maskB |= (1u << x);
        }
        adjAB[a] = maskB;
    }

    for (int bi = 0; bi < n2; bi++) {
        int b = n1 + bi;
        u64 nb = adj[b];

        u32 maskB = 0;
        for (int x = 0; x < n2; x++) {
            int v = n1 + x;
            if (nb & (1ull << v)) maskB |= (1u << x);
        }
        adjB[bi] = maskB;
    }

    // Enumerate cliques in B and build best[mask] = max clique size within mask
    const u32 FULLB = (n2 == 32 ? 0xFFFFFFFFu : ((1u << n2) - 1u));
    const int SZB = 1 << n2;

    vector<u8> cliqueSizeB(SZB, 0);
    vector<u8> isCliqueB(SZB, 0);
    vector<u8> best(SZB, 0);

    isCliqueB[0] = 1;
    cliqueSizeB[0] = 0;
    best[0] = 0;

    for (u32 mask = 1; mask < (u32)SZB; mask++) {
        u32 lsb = mask & -mask;
        u32 rest = mask ^ lsb;
        int v = __builtin_ctz(lsb);

        if (isCliqueB[rest] && ((adjB[v] & rest) == rest)) {
            isCliqueB[mask] = 1;
            cliqueSizeB[mask] = (u8)(cliqueSizeB[rest] + 1);
            best[mask] = cliqueSizeB[mask];
        } else {
            isCliqueB[mask] = 0;
            cliqueSizeB[mask] = 0;
            best[mask] = 0;
        }
    }

    // Subset DP: best[mask] = max(best[submask]) over all submasks of mask
    for (int i = 0; i < n2; i++) {
        for (int mask = 0; mask < SZB; mask++) {
            if (mask & (1 << i)) {
                best[mask] = max(best[mask], best[mask ^ (1 << i)]);
            }
        }
    }

    // Enumerate cliques in A and combine with best compatible clique in B
    const int SZA = 1 << n1;
    vector<u8> isCliqueA(SZA, 0);
    vector<u8> cliqueSizeA(SZA, 0);
    vector<u32> compatB(SZA, 0); // for clique in A: vertices in B adjacent to all in A

    isCliqueA[0] = 1;
    cliqueSizeA[0] = 0;
    compatB[0] = FULLB;

    int ans = 0;

    for (u32 mask = 1; mask < (u32)SZA; mask++) {
        u32 lsb = mask & -mask;
        u32 rest = mask ^ lsb;
        int v = __builtin_ctz(lsb); // v is in [0..n1-1]

        // Clique test in A similarly: v must connect to all in rest
        if (isCliqueA[rest] && ((adjA[v] & rest) == rest)) {
            isCliqueA[mask] = 1;
            cliqueSizeA[mask] = (u8)(cliqueSizeA[rest] + 1);
            compatB[mask] = compatB[rest] & adjAB[v]; // keep only B vertices adjacent to all in A clique

            int candi = (int)cliqueSizeA[mask] + (int)best[compatB[mask]];
            if (candi > ans) ans = candi;
        } else {
            isCliqueA[mask] = 0;
            cliqueSizeA[mask] = 0;
            compatB[mask] = 0;
        }
    }

    // also consider clique entirely in B (A empty)
    ans = max(ans, (int)best[FULLB]);
}

