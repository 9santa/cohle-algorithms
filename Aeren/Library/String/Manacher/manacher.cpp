#include <bits/stdc++.h>

using namespace std;


// p[0][i]: half length of even pal substring around i, p[1][i]: half length(rounded donw) of odd pal substring
// O(n)
template<class Char_Type>
array<vector<int>, 2> manacher(const vector<Char_Type> &s){
	int n = (int)s.size();
	array<vector<int>, 2> p = {vector<int>(n + 1), vector<int>(n)};
	for(auto z = 0; z < 2; ++ z){
		for(auto i = 0, l = 0, r = 0; i < n; ++ i){
			int t = r - i + !z;
			if(i < r) p[z][i] = min(t, p[z][l + t]);
			int L = i - p[z][i], R = i + p[z][i] - !z;
			while(L >= 1 && R + 1 < n && s[L - 1] == s[R + 1]) ++ p[z][i], -- L, ++ R;
			if(R > r) l = L, r = R;
		}
	}
	return p;
}
