package sparsetable

type DisjointSparseTable[T any] struct {
	n    int
	log  int // number of levels
	data [][]T
	op   func(T, T) T
	id   func() T
}

func NewDisjointSparseTable[T any](arr []T, op func(T, T) T, id func() T) *DisjointSparseTable[T] {
	st := &DisjointSparseTable[T]{op: op, id: id}
	st.Build(arr)
	return st
}

// Build from vector.
func (st *DisjointSparseTable[T]) Build(arr []T) {
	st.n = len(arr)
	if st.n == 0 {
		st.log = 0
		st.data = nil
		return
	}

	st.log = 1
	if st.n > 1 {
		st.log = bitsLen(st.n - 1)
	}
	st.data = make([][]T, st.log)
	st.data[0] = append([]T(nil), arr...)

	// build higher levels. each level k uses block size B = 2^k
	for k := 1; k < st.log; k++ {
		B := 1 << k
		st.data[k] = append([]T(nil), st.data[0]...) // start with init values, then overwrite inside each block with prefix/suffix data
		for mid := B; mid < st.n; mid += B << 1 {
			L := mid - B
			R := min(st.n, mid+B)

			// left half: compute suffixes ending at mid-1
			// data[k][i] = a[i] op ... op a[mid-1]
			for i := mid - 2; i >= L; i-- {
				st.data[k][i] = st.op(st.data[0][i], st.data[k][i+1])
			}

			// right half: compute prefixes starting at mid
			// data[k][i] = a[mid] op ... op a[i]
			if mid < R {
				for i := mid + 1; i < R; i++ {
					st.data[k][i] = st.op(st.data[k][i-1], st.data[0][i])
				}
			}
		}
	}
}

// Query [l, r). O(1)
func (st *DisjointSparseTable[T]) Prod(l, r int) T {
	if l >= r {
		return st.id()
	}
	if r-l == 1 {
		return st.data[0][l]
	}
	a := l
	b := r - 1
	// k = msb of (a^b): highest bit where they differ
	k := floorLog2(a ^ b)
	return st.op(st.data[k][a], st.data[k][b])
}
