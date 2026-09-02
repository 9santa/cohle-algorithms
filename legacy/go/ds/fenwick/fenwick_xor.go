package fenwick

type FenwickXor[T Integer] struct {
	n  int
	fw []T
}

func NewFenwickXor[T Integer](n int) *FenwickXor[T] {
	return &FenwickXor[T]{n: n + 1, fw: make([]T, n+1)}
}

func (f *FenwickXor[T]) lsb(i int) int { return i & -i }

// point-xor update: a[i] ^= val
func (f *FenwickXor[T]) Update(i int, val T) {
	for i++; i < f.n; i += f.lsb(i) {
		f.fw[i] ^= val
	}
}

func (f *FenwickXor[T]) Query(i int) T {
	var res T
	for i++; i > 0; i -= f.lsb(i) {
		res ^= f.fw[i]
	}
	return res
}

func (f *FenwickXor[T]) Range(l, r int) T {
	return f.Query(r) ^ f.Query(l-1)
}

func (f *FenwickXor[T]) Get(i int) T {
	return f.Range(i, i)
}

// set a[i] = val
func (f *FenwickXor[T]) Set(i int, val T) {
	cur := f.Get(i)
	f.Update(i, cur^val)
}
