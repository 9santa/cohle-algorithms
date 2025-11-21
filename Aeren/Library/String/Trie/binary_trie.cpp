#include <bits/stdc++.h>

using namespace std;


template<class T, int mx = 30>
struct binary_trie{
	int n = 0;
	vector<array<int, 3>> next{{0, 0, 0}}; // leftchild, rightchild, count
	binary_trie(){ }
	binary_trie(const vector<T> &a){
		for(auto x: a) insert(x);
	}
	void insert(T x){
		++ n;
		for(int bit = mx - 1, u = 0; bit >= 0; -- bit){
			if(!next[u][x >> bit & 1]){
				next[u][x >> bit & 1] = (int)next.size();
				next.push_back({0, 0, 0});
			}
			u = next[u][x >> bit & 1];
			++ next[u][2];
		}
	}
	void erase(T x){
		-- n;
		for(int bit = mx - 1, u = 0; bit >= 0; -- bit){
			assert(next[u][x >> bit & 1]);
			u = next[u][x >> bit & 1];
			assert(next[u][2]);
			-- next[u][2];
		}
	}
	T max_xor(T x){
		if(!n){
			return numeric_limits<T>::min();
		}
		T res = 0;
		for(int bit = mx - 1, u = 0; bit >= 0; -- bit){
			if(!next[u][~x >> bit & 1] || !next[next[u][~x >> bit & 1]][2]) u = next[u][x >> bit & 1];
			else{
				res |= T(1) << bit;
				u = next[u][~x >> bit & 1];
			}
		}
		return res;
	}
	T min_xor(T x){
		if(!n){
			return numeric_limits<T>::max();
		}
		T res = 0;
		for(int bit = mx - 1, u = 0; bit >= 0; -- bit){
			if(!next[u][x >> bit & 1] || !next[next[u][x >> bit & 1]][2]){
				res |= T(1) << bit;
				u = next[u][~x >> bit & 1];
			}
			else u = next[u][x >> bit & 1];
		}
		return res;
	}
};
