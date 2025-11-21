#include <bits/stdc++.h>

using namespace std;


// Requires generate_tree
vector<array<int, 2>> generate_simply_connected_graph(auto &&rng, int n, int m){
	assert(n - 1 <= m && m <= 1LL * n * (n - 1) / 2);
	auto res = generate_tree(rng, n);
	set<array<int, 2>> s;
	for(auto [u, v]: res) s.insert({u, v}), s.insert({v, u});
	for(auto rep = n - 1; rep < m; ++ rep){
		int u, v;
		do{
			u = rng() % n, v = rng() % n;
		}while(u == v || s.count({u, v}));
		s.insert({u, v}), s.insert({v, u});
		res.push_back({u, v});
	}
	return res;
}
