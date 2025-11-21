#include <bits/stdc++.h>

using namespace std;


// Requires generate_tree
vector<array<int, 2>> generate_simply_connected_bipartite_graph(auto &&rng, int n, int trial){
	assert(1 <= n && n - 1 <= trial);
	if(n == 1) return {};
	auto res = generate_tree(rng, n);
	set<array<int, 2>> s;
	vector<vector<int>> adj(n);
	for(auto [u, v]: res) adj[u].push_back(v), adj[v].push_back(u), s.insert({u, v}), s.insert({v, u});
	vector<int> depth(n);
	auto dfs = [&](auto self, int u, int p)->void{
		for(auto v: adj[u]) if(v != p) depth[v] = depth[u] + 1, self(self, v, u);
	};
	dfs(dfs, 0, -1);
	vector<int> left, right;
	for(auto u = 0; u < n; ++ u){
		if(~depth[u] & 1) left.push_back(u);
		else right.push_back(u);
	}
	for(auto i = n - 1; i < trial; ++ i){
		int u = left[rng() % (int)left.size()], v = right[rng() % (int)right.size()];
		if(!s.count({u, v})){
			s.insert({u, v}), s.insert({v, u});
			res.push_back({u, v});
		}
	}
	return res;
}
