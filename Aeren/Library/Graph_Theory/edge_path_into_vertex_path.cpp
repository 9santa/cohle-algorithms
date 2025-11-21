#include <bits/stdc++.h>

using namespace std;


// Requires graph
template<class T>
vector<int> edge_path_into_vertex_path(const graph<T> &g, int s, const vector<int> &edge_path){
	vector<int> vertex_path{s};
	for(auto id: edge_path){
		auto &e = g.edge[id];
		assert(e.from == vertex_path.back() || e.to == vertex_path.back());
		vertex_path.push_back(vertex_path.back() ^ e.from ^ e.to);
	}
	return vertex_path;
}
