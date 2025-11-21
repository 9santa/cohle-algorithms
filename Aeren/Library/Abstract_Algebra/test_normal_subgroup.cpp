#include <bits/stdc++.h>

using namespace std;


// Requires test_closedness
bool test_normal_subgroup(int n, auto op, const vector<int> &s){
	if(s.empty() || !test_closedness(n, op, s)) return false;
	vector<int> in_s(n), inv(n, -1);
	int id = -1;
	for(auto x: s){
		assert(0 <= x && x < n);
		in_s[x] = true;
		if(op(x, x) == x) id = x;
	}
	assert(~id);
	for(auto x = 0; x < n; ++ x) if(!~inv[x]) for(auto y = x; y < n; ++ y) if(op(x, y) == id) inv[x] = y, inv[y] = x;
	for(auto x = 0; x < n; ++ x) if(!in_s[x]) for(auto y: s) if(!in_s[op(op(x, y), inv[x])]) return false;
	return true;
}
bool test_normal_subgroup(const vector<vector<int>> &g, const vector<int> &s){
	return test_normal_subgroup((int)g.size(), [&](int x, int y){ return g[x][y]; }, s);
}
