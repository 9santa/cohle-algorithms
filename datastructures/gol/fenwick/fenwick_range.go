package fenwick

type FenwickRR[T Integer] struct {
	n   int
	fw1 []T
	fw2 []T
}

func NewFenwickRR[T Integer](n int) *FenwickRR[T] {
	return &FenwickRR[T]{
		n:   n + 1,
		fw1: make([]T, n+1),
		fw2: make([]T, n+1),
	}
}

func (f *FenwickRR[T]) lsb(i int) int { return i & -i }

func (f *FenwickRR[T]) pointAdd(fw []T, i int, val T) {
	for i++; i < f.n; i += f.lsb(i) {
		fw[i] += val
	}
}

func (f *FenwickRR[T]) pointQuery(fw []T, i int) T {
	var res T
	for i++; i > 0; i -= f.lsb(i) {
		res += fw[i]
	}
	return res
}

// add val to [l..r]
func (f *FenwickRR[T]) RangeAdd(l, r int, val T) {
	f.pointAdd(f.fw1, l, val)
	f.pointAdd(f.fw1, r+1, -val)
	f.pointAdd(f.fw2, l, val*T(l-1))
	f.pointAdd(f.fw2, r+1, -val*T(r))
}

// prefix sum [0..i]
func (f *FenwickRR[T]) Prefix(i int) T {
	if i < 0 {
		var zero T
		return zero
	}
	return f.pointQuery(f.fw1, i)*T(i+1) - f.pointQuery(f.fw2, i)
}

// range sum [l..r]
func (f *FenwickRR[T]) RangeQuery(l, r int) T {
	if l > r {
		var zero T
		return zero
	}
	return f.Prefix(r) - f.Prefix(l-1)
}
