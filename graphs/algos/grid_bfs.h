#include "../base.h"

static constexpr int DX4[4] = {1, -1, 0, 0};
static constexpr int DY4[4] = {0, 0, 1, -1};

// Single-source BFS on grid. passable(x, y) decides if cell can be entered
template<class Passable>
V<vi> bfs_grid_4(int H, int W, pii start, Passable passable) {
    V<vi> dist(H, vi(W, -1));
    queue<pii> q;

    auto [sx, sy] = start;
    if (sx < 0 || sx >= H || sy < 0 || sy >= W) return dist;
    if (!passable(sx, sy)) return dist;

    dist[sx][sy] = 0;
    q.push({sx, sy});

    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        for (int k = 0; k < 4; k++) {
            int nx = x + DX4[k], ny = y + DY4[k];
            if (nx < 0 || nx >= H || ny < 0 || ny >= W) continue;
            if (!passable(nx, ny)) continue;
            if (dist[nx][ny] != -1) continue;
            dist[nx][ny] = dist[x][y] + 1;
            q.push({nx, ny});
        }
    }
    return dist;
}

// Multi-source BFS on grid
template<class Passable>
V<vi> multi_source_bfs_grid_4(int H, int W, const V<pii>& sources, Passable passable) {
    V<vi> dist(H, vi(W, -1));
    queue<pii> q;

    for (auto [sx, sy] : sources) {
        if (sx < 0 || sx >= H || sy < 0 || sy >= W) continue;
        if (!passable(sx, sy)) continue;
        if (dist[sx][sy] != -1) continue;
        dist[sx][sy] = 0;
        q.push({sx, sy});
    }

    while (!q.empty()) {
        auto [x, y] = q.front(); q.pop();
        for (int k = 0; k < 4; k++) {
            int nx = x + DX4[k], ny = y + DY4[k];
            if (nx < 0 || nx >= H || ny < 0 || ny >= W) continue;
            if (!passable(nx, ny)) continue;
            if (dist[nx][ny] != -1) continue;
            dist[nx][ny] = dist[x][y] + 1;
            q.push({nx, ny});
        }
    }
    return dist;
}

/* Example: grid[x][y] != 0 passable
   auto dist = bfs_grid_4(H, W, {sx, sy}, [&](int x, int y) { return grid[x][y] != 0; }); */
