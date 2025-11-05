#include <algorithm>
#include <bits/stdc++.h>
#include <vector>

using namespace std;


[[nodiscard("cycle detection")]] bool topologicalSortDfs(const vector<vector<int>>& graph, vector<int>& visited, vector<int>& topsort, int v) {
    visited[v] = 1; // currently in recursion stack (visiting)

    for(auto u : graph[v]) {
        if(visited[u] == 1) {
            return true;    // cycle detected
        }
        else if(visited[u] == 0) {
            if(topologicalSortDfs(graph, visited, topsort, u)) {
                return true;
            }
        }
    }

    visited[v] = 2;
    topsort.push_back(v);
    return false;
}


// test example
int main(void)
{
    clock_t start = clock();
    ios::sync_with_stdio(0);
    cin.tie(0);

    vector<vector<int>> graph = {
        {1, 3, 4},
        {2},
        {},
        {2},
        {2, 3}
    };

    vector<int> visited(graph.size(), 0);
    vector<int> topsort;
    bool has_cycle = false;

    for(int i = 0; i < (int)graph.size(); i++) {
        if(visited[i] == 0) {
            if(topologicalSortDfs(graph, visited, topsort, i)) {
                has_cycle = true;
                break;
            }
        } 
    }

    if(!has_cycle) {
        reverse(topsort.begin(), topsort.end());
        cout << "Topological order: ";
        for(auto& el : topsort) cout << el+1 << " ";
    } else cout << "Graph contains a cycle! Topological sort is not possible.\n";


    double run_time = (double)(clock() - start) / CLOCKS_PER_SEC;
	cerr << "\nRun Time : " << std::fixed << std::setprecision(6) << run_time << " seconds\n";

    return 0;
}
