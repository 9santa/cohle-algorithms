package fenwick

type FenwickCounter[T Integer] struct {
	n  int
	fw []T
}

func NewFenwickCounter[T Integer](n int) *FenwickCounter[T] {
	return &FenwickCounter[T]{n: n, fw: make([]T, n+1)}
}

func (f *FenwickCounter[T]) lsb(i int) int { return i & -i }

func (f *FenwickCounter[T]) PointAdd(i int, delta T) {
	for ; i > 0; i -= f.lsb(i) {
		f.fw[i] += delta
	}
}

func (f *FenwickCounter[T]) Suf(i int) int64 {
	var res int64
	for ; i <= f.n; i += f.lsb(i) {
		res += int64(f.fw[i])
	}
	return res
}

func (f *FenwickCounter[T]) RangeSum(l, r int) int64 {
	if l > r {
		return 0
	}
	res := f.Suf(l)
	if r+1 <= f.n {
		res -= f.Suf(r + 1)
	}
	return res
}
