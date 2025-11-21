#include <bits/stdc++.h>

using namespace std;


// Duval algorithm for computing lyndon factorization
// Return the list of starting indices
// O(n)
template<class Char_Type>
vector<int> lyndon_factorization(const vector<Char_Type> &s){
	int n = (int)s.size();
	vector<int> res;
	for(auto i = 0; i < n; ){
		int l = i, r = i + 1;
		for(; r < n && s[l] <= s[r]; ++ r) s[l] < s[r] ? l = i : ++ l;
		for(; i <= l; i += r - l) res.push_back(i);
	}
	return res;
}
