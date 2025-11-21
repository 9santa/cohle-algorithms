#include <bits/stdc++.h>

using namespace std;


// Assumes s is a subset.
// At most n op() calls.
vector<vector<int>> cosets(int n, auto op, const vector<int> &s){
	vector<vector<int>> res{s};
	vector<int> in_res(n);
	for(auto x: s) in_res[x] = true;
	for(auto x = 0; x < n; ++ x) if(!in_res[x]){
		res.emplace_back();
		for(auto y: s){
			int z = op(x, y);
			assert(!in_res[z]);
			res.back().push_back(z);
			in_res[z] = true;
		}
	}
	return res;
}
vector<vector<int>> cosets(const vector<vector<int>> &g, const vector<int> &s){
	return cosets((int)g.size(), [&](int x, int y){ return g[x][y]; }, s);
}
