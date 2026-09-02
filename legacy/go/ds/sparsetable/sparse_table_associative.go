package sparsetable

/*
Sparse Table for associative Monoid (doesn't require idempotency)

	Query: O(log n) with binary decomp
	Preserves order: res = op(res, block), so non-commutative is fine
	Memory: O(n log n), without padding to power-of-two
*/
type SparseTableMonoidDecomp[T any] struct {
	n   int // original size
	log int // number of levels
	st  [][]T
	op  func(T, T) T
	id  func() T
}

func NewSparseTableMonoidDecomp[T any](arr []T, op func(T, T) T, id func() T) *SparseTableMonoidDecomp[T] {
	st := &SparseTableMonoidDecomp[T]{op: op, id: id}
	st.Build(arr)
	return st
}

// Build from array.
func (st *SparseTableMonoidDecomp[T]) Build(arr []T) {
	st.n = len(arr)
	if st.n == 0 {
		st.log = 0
		st.st = nil
		return
	}

	st.log = bitsLen(st.n)
	st.st = make([][]T, st.log)
	st.st[0] = append([]T(nil), arr...)

	// level k stores segments of length 2^k
	// so it has size m = n - 2^k + 1
	for k := 1; k < st.log; k++ {
		length := 1 << k
		half := length >> 1
		m := st.n - length + 1
		if m < 0 {
			m = 0
		}
		st.st[k] = make([]T, m)
		for i := 0; i < m; i++ {
			st.st[k][i] = st.op(st.st[k-1][i], st.st[k-1][i+half])
		}
	}
}

// Query [l, r). O(log n)
func (st *SparseTableMonoidDecomp[T]) Prod(l, r int) T {
	if l < 0 {
		l = 0
	}
	if r > st.n {
		r = st.n
	}
	if l >= r {
		return st.id()
	}

	res := st.id()
	length := r - l
	for length > 0 {
		k := floorLog2(length) // largest k: 2^k <= len
		res = st.op(res, st.st[k][l])
		l += 1 << k
		length -= 1 << k
	}
	return res
}

func bitsLen(x int) int {
	return floorLog2(x) + 1
}
