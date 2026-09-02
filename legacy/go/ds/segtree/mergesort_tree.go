package segtree

import "sort"

type MergesortTree[T Ordered] struct {
	n int
	t [][]T
}

func NewMergesortTree[T Ordered](a []T) *MergesortTree[T] {
	mt := &MergesortTree[T]{}
	mt.Build(a)
	return mt
}

func (mt *MergesortTree[T]) Build(a []T) {
	mt.n = len(a)
	mt.t = make([][]T, 2*mt.n)
	for i := 0; i < mt.n; i++ {
		mt.t[mt.n+i] = []T{a[i]}
	}
	for i := mt.n - 1; i > 0; i-- {
		mt.t[i] = mergeSorted(mt.t[i<<1], mt.t[i<<1|1])
	}
}

// count elements <= x in [l, r). O(log^2(n))
func (mt *MergesortTree[T]) CountLessOrEqual(l, r int, x T) int {
	mt.checkRange(l, r)
	res := 0
	for l, r = l+mt.n, r+mt.n; l < r; l, r = l>>1, r>>1 {
		if l&1 == 1 {
			res += upperBound(mt.t[l], x)
			l++
		}
		if r&1 == 1 {
			r--
			res += upperBound(mt.t[r], x)
		}
	}
	return res
}

// O(log^2(n))
func (mt *MergesortTree[T]) CountLess(l, r int, x T) int {
	mt.checkRange(l, r)
	res := 0
	for l, r = l+mt.n, r+mt.n; l < r; l, r = l>>1, r>>1 {
		if l&1 == 1 {
			res += lowerBound(mt.t[l], x)
			l++
		}
		if r&1 == 1 {
			r--
			res += lowerBound(mt.t[r], x)
		}
	}
	return res
}

// O(log^2(n))
func (mt *MergesortTree[T]) CountGreaterOrEqual(l, r int, x T) int {
	mt.checkRange(l, r)
	res := 0
	for l, r = l+mt.n, r+mt.n; l < r; l, r = l>>1, r>>1 {
		if l&1 == 1 {
			res += len(mt.t[l]) - lowerBound(mt.t[l], x)
			l++
		}
		if r&1 == 1 {
			r--
			res += len(mt.t[r]) - lowerBound(mt.t[r], x)
		}
	}
	return res
}

// O(log^2(n))
func (mt *MergesortTree[T]) CountGreater(l, r int, x T) int {
	mt.checkRange(l, r)
	res := 0
	for l, r = l+mt.n, r+mt.n; l < r; l, r = l>>1, r>>1 {
		if l&1 == 1 {
			res += len(mt.t[l]) - upperBound(mt.t[l], x)
			l++
		}
		if r&1 == 1 {
			r--
			res += len(mt.t[r]) - upperBound(mt.t[r], x)
		}
	}
	return res
}

// O(log^2(n))
func (mt *MergesortTree[T]) CountWithin(l, r int, xl, xr T) int {
	mt.checkRange(l, r)
	res := 0
	for l, r = l+mt.n, r+mt.n; l < r; l, r = l>>1, r>>1 {
		if l&1 == 1 {
			res += lowerBound(mt.t[l], xr) - lowerBound(mt.t[l], xl)
			l++
		}
		if r&1 == 1 {
			r--
			res += lowerBound(mt.t[r], xr) - lowerBound(mt.t[r], xl)
		}
	}
	return res
}

// O(SZ * log n) where SZ is the size of the answer
func (mt *MergesortTree[T]) GetSortedList(l, r int) []T {
	mt.checkRange(l, r)
	res := make([]T, 0)
	for l, r = l+mt.n, r+mt.n; l < r; l, r = l>>1, r>>1 {
		if l&1 == 1 {
			res = mergeSorted(res, mt.t[l])
			l++
		}
		if r&1 == 1 {
			r--
			res = mergeSorted(mt.t[r], res)
		}
	}
	return res
}

func (mt *MergesortTree[T]) checkRange(l, r int) {
	if !(0 <= l && l <= r && r <= mt.n) {
		panic("invalid range")
	}
}

func lowerBound[T Ordered](a []T, x T) int {
	return sort.Search(len(a), func(i int) bool { return a[i] >= x })
}

func upperBound[T Ordered](a []T, x T) int {
	return sort.Search(len(a), func(i int) bool { return a[i] > x })
}

func mergeSorted[T Ordered](a, b []T) []T {
	res := make([]T, 0, len(a)+len(b))
	i, j := 0, 0
	for i < len(a) && j < len(b) {
		if a[i] <= b[j] {
			res = append(res, a[i])
			i++
		} else {
			res = append(res, b[j])
			j++
		}
	}
	res = append(res, a[i:]...)
	res = append(res, b[j:]...)
	return res
}
