#include <bits/stdc++.h>

using namespace std;


// Returns # of distinct non-empty substrings
// O(n)
// Requires suffix_automaton
template<class Char_Type, class Adjacency_Type>
long long count_distinct_substrings(const vector<Char_Type> &s){
	suffix_automaton<Char_Type, Adjacency_Type> aut;
	aut.extend(s);
	long long res = 0;
	for(auto u = 1; u < (int)aut.size(); ++ u) res += aut.max_len[u] - aut.max_len[aut.link[u]];
	return res;
}
