#include <bits/stdc++.h>

using namespace std;


template<class T>
struct logging_allocator{
	using value_type = T;
	allocator<T> a;
	logging_allocator() = default;
	template<class U>
	logging_allocator(const logging_allocator<U>&) {}
	T *allocate(size_t n){
		cerr << "Allocating " << n << " bytes." << endl;
		return a.allocate(n);
	}
	void deallocate(T *ptr, size_t n) {
		cerr << "Deallocating " << n << " bytes." << endl;
		return a.deallocate(ptr, n);
	}
};
