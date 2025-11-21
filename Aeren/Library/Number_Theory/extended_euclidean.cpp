#include <bits/stdc++.h>

using namespace std;


// Returns g=gcd(a,b), and x and y store values with ax+by=g
// O(log(min(abs(a), abs(b))))
template<class T>
T extended_euclidean(T a, T b, T &x, T &y){
	if(a < 0) a = -a;
	if(b < 0) b = -b;
	x = 1, y = 0;
	for(T q, r, u = 0, v = 1; b; ){
		q = a / b, r = a % b;
		tie(a, b, x, y, u, v) = tuple{b, r, u, v, x - u * q, y - v * q};
	}
	return a;
}
