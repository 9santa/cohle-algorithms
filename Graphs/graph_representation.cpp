#include <bits/stdc++.h>
#include <ctime>

#define pb push_back
#define eb emplace_back
#define all(a) (a).begin(), (a).end()

using namespace std;

using ll = long long;
using ull = unsigned long long;
using ld = long double;
using pii = pair<int, int>;

// 3 ways to represent a Graph
// 1. Adjacency list
class GraphList {
private:
    int V;  // number of vertices
    vector<vector<pair<int, int>>> adj_list;    // pair: {neighbor, weight}
    bool directed;

public:
    GraphList(int vertices, bool is_directed = false) : V(vertices), directed(is_directed) {
        adj_list.resize(V);
    }
};



int main(void)
{
    clock_t start = clock();
    ios::sync_with_stdio(0);
    cin.tie(0);



    double run_time = (double)(clock() - start) / CLOCKS_PER_SEC;
	cerr << "\nRun Time : " << std::fixed << std::setprecision(6) << run_time << " seconds\n";

    return 0;
}
