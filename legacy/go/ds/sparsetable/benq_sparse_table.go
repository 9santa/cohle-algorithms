package sparsetable

import "fmt"

// 1D range min queries. O(1) time per query, O(N log N) preprocessing, O(N log N) mem
// immutable array, sparse table
type RMQ[T Ordered] struct {
	v   []T
	jmp [][]int // jmp = sparse table with indices instead of values
}

func (r *RMQ[T]) level(x int) int {
	return floorLog2(x) // max x: 2^x <= r-l, this is the leftmost set bit for segment (r-l)
}

// get index of the min value
func (r *RMQ[T]) cmb(a, b int) int {
	if r.v[a] == r.v[b] {
		return min(a, b)
	}
	if r.v[a] < r.v[b] {
		return a
	}
	return b
}

func NewRMQ[T Ordered](v []T) *RMQ[T] {
	r := &RMQ[T]{}
	r.Init(v)
	return r
}

func (r *RMQ[T]) Init(v []T) {
	r.v = append([]T(nil), v...)
	r.jmp = [][]int{make([]int, len(v))}
	for i := range r.jmp[0] {
		r.jmp[0][i] = i // index of the min value of length 1 - is 'i' itself
	}
	for j := 1; 1<<j <= len(r.v); j++ {
		level := make([]int, len(r.v)-(1<<j)+1)
		for i := range level {
			// split into two parts in the middle: 2^j = 2 * 2^(j-1)
			level[i] = r.cmb(r.jmp[j-1][i], r.jmp[j-1][i+(1<<(j-1))])
		}
		r.jmp = append(r.jmp, level)
	}
}

func (r *RMQ[T]) Index(l, rr int) int {
	if l > rr {
		panic(fmt.Sprintf("Index: invalid range [%d, %d]", l, rr))
	}
	d := r.level(rr - l + 1)
	return r.cmb(r.jmp[d][l], r.jmp[d][rr-(1<<d)+1])
}

func (r *RMQ[T]) Query(l, rr int) T {
	return r.v[r.Index(l, rr)]
}
