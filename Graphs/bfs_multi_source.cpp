#include <bits/stdc++.h>


using namespace std;

vector<vector<int>> multi_source_bfs(const vector<vector<int>>& grid, const vector<pair<int, int>>& sources) {
    int rows = (int)grid.size();
    int cols = (int)grid[0].size();

    vector<vector<int>> dist(rows, vector<int>(cols, -1));
    queue<pair<int, int>> q;

    // add all sources to queue
    for(auto [x, y] : sources) {
        if(grid[x][y] != 0) {
            dist[x][y] = 0;
            q.push({x, y});
        }
    }

    vector<pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

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


int main(void)
{
    clock_t start = clock();
    ios::sync_with_stdio(0);
    cin.tie(0);



    double run_time = (double)(clock() - start) / CLOCKS_PER_SEC;
	cerr << "\nRun Time : " << std::fixed << std::setprecision(6) << run_time << " seconds\n";

    return 0;
}
