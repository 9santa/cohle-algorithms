
vector<vector<int>> bfs_grid(const vector<vector<int>>& grid, pair<int, int> start) {
    int rows = (int)grid.size();
    int cols = (int)grid[0].size();

    vector<vector<int>> dist(rows, vector<int>(cols, -1));
    queue<pair<int, int>> q;

    vector<pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    q.push(start);
    dist[start.first][start.second] = 0;

    while(!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        for(auto [dx, dy] : directions) {
            int nx = x + dx;
            int ny = y + dy;

            if(nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
                if(grid[nx][ny] != 0 && dist[nx][ny] == -1) {
                    dist[nx][ny] = dist[x][y] + 1;
                    q.push({nx, ny});
                }
            }
        }
    }

    return dist;
}
