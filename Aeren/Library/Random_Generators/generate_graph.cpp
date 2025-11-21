#include <bits/stdc++.h>

using namespace std;


vector<array<int, 2>> generate_graph(auto &&rng, int n, int m){
	vector<array<int, 2>> res(m);
	for(auto &[u, v]: res) u = rng() % n, v = rng() % n;
	return res;
}
