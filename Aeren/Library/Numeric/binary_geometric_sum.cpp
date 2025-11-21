#include <bits/stdc++.h>

using namespace std;


// Returns x + x * b + ... + x * b^(e-1)
// O(log(e))
template<class T, class U, class V>
T binary_geometric_sum(T x, U b, V e){
	assert(e >= 0);
	T res{}; // This should be the additive identity
	for(; e; e >>= 1){
		if(e & 1){
			res = x + res;
			x *= b;
		}
		x += x * b;
		b *= b;
	}
	return res;
}
