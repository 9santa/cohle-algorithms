#include <bits/stdc++.h>

using namespace std;


// O(n + m * sqrt(m)) for graphs without loops or multiedges
template<class T>
T count_4_cycles(int n, const vector<array<int, 2>> &edge){
	int m = (int)edge.size();
	vector<int> deg(n), order, pos(n);
	vector<vector<int>> appear(m + 1), adj(n);
	for(auto [u, v]: edge) ++ deg[u], ++ deg[v];
	for(auto u = 0; u < n; ++ u) appear[deg[u]].push_back(u);
	for(auto d = m; d >= 0; -- d) order.insert(order.end(), appear[d].begin(), appear[d].end());
	for(auto i = 0; i < n; ++ i) pos[order[i]] = i;
	for(auto i = 0; i < m; ++ i){
		int u = pos[edge[i][0]], v = pos[edge[i][1]];
		adj[u].push_back(v), adj[v].push_back(u);
	}
	T res = 0;
	vector<int> cnt(n);
	for(auto u = 0; u < n; ++ u){
		for(auto v: adj[u]) if(u < v) for(auto w: adj[v]) if(u < w) cnt[w] = 0;
		for(auto v: adj[u]) if(u < v) for(auto w: adj[v]) if(u < w) res += cnt[w] ++;
	}
	return res;
}
