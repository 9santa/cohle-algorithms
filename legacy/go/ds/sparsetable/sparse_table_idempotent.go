package sparsetable

// Templated version for different idempotent operations
// better(x, y) -> bool (true if x is strictly better)!!!
type SparseTableIdempotent[T any] struct {
	n      int
	k      int
	table  []int // size k*n, stores indices
	arr    []T   // bind to original array copy
	better func(T, T) bool
	id     func() T
}

func NewSparseTableIdempotent[T any](arr []T, better func(T, T) bool, id func() T) *SparseTableIdempotent[T] {
	st := &SparseTableIdempotent[T]{better: better, id: id}
	st.Build(arr)
	return st
}

func (st *SparseTableIdempotent[T]) Build(arr []T) {
	st.arr = append([]T(nil), arr...)
	st.n = len(arr)
	if st.n == 0 {
		st.k = 0
		st.table = nil
		return
	}
	st.k = bitsLen(st.n) // ceil-ish log2 table height as in the original
	st.table = make([]int, st.k*st.n)

	// Initialize first level
	for i := 0; i < st.n; i++ {
		st.table[i] = i
	}

	// Build the table
	for j := 1; j < st.k; j++ {
		step := 1 << (j - 1)
		for i := 0; i+(1<<j) <= st.n; i++ {
			x := st.table[(j-1)*st.n+i]
			y := st.table[(j-1)*st.n+i+step]
			if st.better(st.arr[x], st.arr[y]) {
				st.table[j*st.n+i] = x
			} else {
				st.table[j*st.n+i] = y
			}
		}
	}
}

func (st *SparseTableIdempotent[T]) QueryIndex(l, r int) int {
	if !(0 <= l && l <= r && r < st.n) {
		panic("QueryIndex: invalid range")
	}
	length := r - l + 1
	j := floorLog2(length)
	x := st.table[j*st.n+l]
	y := st.table[j*st.n+r-(1<<j)+1]
	if st.better(st.arr[x], st.arr[y]) {
		return x
	}
	return y
}

// Query [l, r] (0-indexed, inclusive)
func (st *SparseTableIdempotent[T]) Query(l, r int) T {
	if st.n == 0 || l > r {
		return st.id()
	}
	if l < 0 {
		l = 0
	}
	if r >= st.n {
		r = st.n - 1
	}
	return st.arr[st.QueryIndex(l, r)]
}
