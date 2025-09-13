#include <algorithm>
#include <bits/stdc++.h>
#include <climits>
#include <vector>

using namespace std;

class TravellingSalesmanProblem {
public:
    static void tsp(int n, const vector<vector<int>>& dist) {
        int num_masks = 1 << n;
        vector<vector<int>> dp(num_masks, vector<int>(n, INT_MAX));
        vector<vector<int>> parent(num_masks, vector<int>(n, -1));

        // base case
        dp[1][0] = 0;

        // fill dp
        for(int mask = 1; mask < num_masks; mask++) {
            for(int i = 0; i < n; i++) {
                if((mask & (1 << i)) == 0 || dp[mask][i] == INT_MAX) {
                    continue;
                }

                for(int j = 0; j < n; j++) {
                    if(mask & (1 << j)) {
                        continue;
                    }

                    int new_mask = mask | (1 << j);
                    int new_cost = dp[mask][i] + dist[i][j];

                    if(new_cost < dp[new_mask][j]) {
                        dp[new_mask][j] = new_cost;
                        parent[new_mask][j] = i;    // memorize where we came from
                    }
                }
            }
        }

        // find minimum cost and last city
        int full_mask = (1 << n) - 1;
        int min_cost = INT_MAX;
        int last_city = -1;

        for(int i = 0; i < n; i++) {
            if(dp[full_mask][i] != INT_MAX) {
                int total_cost = dp[full_mask][i] + dist[i][0];
                if(total_cost < min_cost) {
                    min_cost = total_cost;
                    last_city = i;
                }
            }
        }

        // reconstruct path
        vector<int> path;
        int current_mask = full_mask;
        int current_city = last_city;

        while(current_city != -1) {
            path.push_back(current_city);
            int prev_city = parent[current_mask][current_city];
            if(prev_city == -1) break;

            // removing current city from bitmask
            current_mask = current_mask & ~(1 << current_city);
            current_city = prev_city;
        }

        // add start city to finish the path
        path.push_back(0);
        reverse(path.begin(), path.end());

        // print results
        cout << "Minimum cost: " << min_cost << "\n";
        cout << "Path: ";
        for(int city : path) {
            cout << city << " ";
        }
        cout << "\n";
    }
};

int main(void) {
    // city count
    int n = 4;

    // distance matrix
    vector<vector<int>> dist = {
        {0, 10, 15, 20},
        {10, 0, 35, 25},
        {15, 35, 0, 30},
        {20, 25, 30, 0}
    };

    TravellingSalesmanProblem::tsp(n, dist);



    return 0;
}
