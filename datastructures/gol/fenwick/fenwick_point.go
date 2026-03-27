package fenwick

import "math/bits"

type FenwickPoint[T Integer] struct {
	n  int
	fw []T
}

func NewFenwickPoint[T Integer](n int) *FenwickPoint[T] {
	return &FenwickPoint[T]{n: n + 1, fw: make([]T, n+1)}
}

func (f *FenwickPoint[T]) lsb(i int) int { return i & -i }

func (f *FenwickPoint[T]) Build(a []T) {
	f.n = len(a) + 1
	f.fw = make([]T, f.n)
	for i := 1; i < f.n; i++ {
		f.fw[i] += a[i-1]
		if i+f.lsb(i) <= len(a) {
			f.fw[i+f.lsb(i)] += f.fw[i]
		}
	}
}

func (f *FenwickPoint[T]) Update(i int, val T) {
	for i++; i < f.n; i += f.lsb(i) {
		f.fw[i] += val
	}
}

func (f *FenwickPoint[T]) Get(i int) T {
	var res T
	for i++; i > 0; i -= f.lsb(i) {
		res += f.fw[i]
	}
	return res
}

// [l, r)
func (f *FenwickPoint[T]) GetRange(l, r int) T {
	return f.Get(r) - f.Get(l-1)
}

// 0-based index of first prefix sum >= x
// prefix sums have to be monotone for this
func (f *FenwickPoint[T]) LowerBound(x T) int {
	pos := 0
	var sum T
	for i := bits.Len(uint(f.n)) - 1; i >= 0; i-- {
		nxt := pos + (1 << i)
		if nxt < f.n && sum+f.fw[nxt] < x {
			sum += f.fw[nxt]
			pos = nxt
		}
	}
	return pos
}

/* If at the very start (1..16) was already too big, then reaching index 16 is impossible while keeping sum < x.
However, we can still go somewhere between 8 and 16 (like 12, 14, 15...) by taking smaller block. */
