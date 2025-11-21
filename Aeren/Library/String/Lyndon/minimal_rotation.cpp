#include <bits/stdc++.h>

using namespace std;


// Returns the position of the lexicographically minimal rotation
// O(n)
template<class Char_Type>
int minimal_rotation(vector<Char_Type> s){
	int n = (int)s.size(), minpos = 0;
	for(auto i = 0; i < n; ++ i) s.push_back(s[i]);
	for(auto pos = 0; pos < n; ++ pos) for(auto i = 0; i < n; ++ i){
		if(minpos + i == pos || s[minpos + i] < s[pos + i]){
			pos += max(0, i - 1);
			break;
		}
		if(s[minpos + i] > s[pos + i]){
			minpos = pos;
			break;
		}
	}
	return minpos;
}
