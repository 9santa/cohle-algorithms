#include <bits/stdc++.h>

using namespace std;


// O(n + m * sqrt(m) + th) for graphs without loops or multiedges
void find_4_cycles(int n, const vector<array<int, 2>> &edge, auto process, int th = 1){
	int m = (int)edge.size();
	vector<int> deg(n), order, pos(n);
	vector<vector<int>> appear(m + 1), adj(n), found(n);
	for(auto [u, v]: edge) ++ deg[u], ++ deg[v];
	for(auto u = 0; u < n; ++ u) appear[deg[u]].push_back(u);
	for(auto d = m; d >= 0; -- d) order.insert(order.end(), appear[d].begin(), appear[d].end());
	for(auto i = 0; i < n; ++ i) pos[order[i]] = i;
	for(auto i = 0; i < m; ++ i){
		int u = pos[edge[i][0]], v = pos[edge[i][1]];
		adj[u].push_back(v), adj[v].push_back(u);
	}
	for(auto u = 0; u < n; ++ u){
		for(auto v: adj[u]) if(u < v) for(auto w: adj[v]) if(u < w) found[w].clear();
		for(auto v: adj[u]) if(u < v) for(auto w: adj[v]) if(u < w){
			for(auto x: found[w]){
				if(!(th --)) return;
				process(order[u], order[v], order[w], order[x]);
			}
			found[w].push_back(v);
		}
	}
}
