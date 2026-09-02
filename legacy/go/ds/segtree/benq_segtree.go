package segtree

// segment tree for associative operation 'cmb'. O(log n) query, O(n) preprocessing
type BenqSegTree[T any] struct {
	ID  T
	Cmb func(T, T) T // ID - identity (neutral) element
	n   int
	seg []T
}

func NewBenqSegTree[T any](n int, id T, cmb func(T, T) T) *BenqSegTree[T] {
	st := &BenqSegTree[T]{ID: id, Cmb: cmb}
	st.Init(n)
	return st
}

func (st *BenqSegTree[T]) Init(n int) {
	for st.n = 1; st.n < n; st.n *= 2 {
	}
	st.seg = make([]T, 2*st.n)
	for i := range st.seg {
		st.seg[i] = st.ID
	}
}

func (st *BenqSegTree[T]) pull(p int) {
	st.seg[p] = st.Cmb(st.seg[2*p], st.seg[2*p+1])
}

// set val at position p
func (st *BenqSegTree[T]) Upd(p int, val T) {
	p += st.n
	st.seg[p] = val
	for p /= 2; p > 0; p /= 2 {
		st.pull(p)
	}
}

// 0-indexed, inclusive
func (st *BenqSegTree[T]) Query(l, r int) T {
	ra, rb := st.ID, st.ID
	for l, r = l+st.n, r+st.n+1; l < r; l, r = l/2, r/2 {
		if l&1 == 1 {
			ra = st.Cmb(ra, st.seg[l])
			l++
		}
		if r&1 == 1 {
			r--
			rb = st.Cmb(st.seg[r], rb)
		}
	}
	return st.Cmb(ra, rb)
}
