#include <bits/stdc++.h>

using namespace std;


vector<array<int, 2>> generate_simple_graph(auto &&rng, int n, int m){
	assert(1 <= n && 0 <= m && m <= 1LL * n * (n - 1) / 2);
	vector<array<int, 2>> res(m);
	set<array<int, 2>> s;
	for(auto &[u, v]: res){
		do{
			u = rng() % n, v = rng() % n;
		}while(u == v || s.count({u, v}) || s.count({v, u}));
		s.insert({u, v}), s.insert({v, u});
	}
	return res;
}
