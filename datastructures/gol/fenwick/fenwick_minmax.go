package fenwick

import "math"

type FenwickMin struct {
	n  int
	fw []int
	id int
}

func NewFenwickMin(n int) *FenwickMin {
	fw := make([]int, n+1)
	for i := range fw {
		fw[i] = math.MaxInt
	}
	return &FenwickMin{n: n + 1, fw: fw, id: math.MaxInt}
}

func (f *FenwickMin) lsb(i int) int { return i & -i }

func (f *FenwickMin) Update(i, val int) {
	for i++; i < f.n; i += f.lsb(i) {
		f.fw[i] = min(f.fw[i], val)
	}
}

func (f *FenwickMin) Query(i int) int {
	res := f.id
	for i++; i > 0; i -= f.lsb(i) {
		res = min(res, f.fw[i])
	}
	return res
}

type FenwickMax struct {
	n  int
	fw []int
	id int
}

func NewFenwickMax(n int) *FenwickMax {
	fw := make([]int, n+1)
	for i := range fw {
		fw[i] = math.MinInt
	}
	return &FenwickMax{n: n + 1, fw: fw, id: math.MinInt}
}

func (f *FenwickMax) lsb(i int) int { return i & -i }

func (f *FenwickMax) Update(i, val int) {
	for i++; i < f.n; i += f.lsb(i) {
		f.fw[i] = max(f.fw[i], val)
	}
}

func (f *FenwickMax) Query(i int) int {
	res := f.id
	for i++; i > 0; i -= f.lsb(i) {
		res = max(res, f.fw[i])
	}
	return res
}
