#include <bits/stdc++.h>

using namespace std;


// Support insertion in a static data structure, assuming query results can be obtained from its disjoint subsets
template<class B, class C>
struct data_structure_insertion_enabler_online{
	int n; // Elements should lie in range [0, n).
	vector<int> element;
	B build; // build(id, a): build the DS with updates in a and assign it to bucket id.
	C clear; // clear(id): assign empty DS to bucket id.
	data_structure_insertion_enabler_online(int n, B build, C clear): n(n), build(build), clear(clear){
		assert(n >= 0);
	}
	// Insert update i.
	// Amortized O(clear() + build(n) / n)
	void insert(int i){
		assert(0 <= i && i < n);
		element.push_back(i);
		int id = __builtin_ctz((int)element.size());
		build(id, {(int)element.size() - (1 << id), (int)element.size()});
		for(auto idprev = 0; idprev < id; ++ idprev) clear(idprev);
	}
};
