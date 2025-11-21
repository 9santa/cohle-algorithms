#include <bits/stdc++.h>

using namespace std;


// binary exponentiation
// O(log e)
template<class T, class U>
T binary_power(T b, U e){
	T res = 1;
	for(; e; e >>= 1, b *= b) if(e & 1) res *= b;
	return res;
}
