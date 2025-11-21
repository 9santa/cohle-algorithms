#include <bits/stdc++.h>

using namespace std;


// O(log(i))
template<class T>
T fibonacci(auto i){
	assert(i >= 0);
	if(i <= 10){
		if(i < 2) return i;
		T a = 0, b = 1;
		for(; i >= 2; -- i){
			swap(a, b);
			b += a;
		}
		return b;
	}
	T a = i % 2, b = 1 - 2 * (i % 2), c = 3;
	for(i >>= 1; i >= 2; i >>= 1){
		if(i % 2 == 0) b = a + b * c;
		else a = b + a * c;
		c = c * c - 2;
	}
	return b + a * c;
}
