package segtree

// Counting SegTree for frequency of some value 'x'
type CountingSegTree struct {
	t []int
	n int
	x int
}

func NewCountingSegTree(a []int, x int) *CountingSegTree {
	st := &CountingSegTree{
		t: make([]int, 4*max(1, len(a))),
		n: len(a),
		x: x,
	}
	if len(a) > 0 {
		st.build(a, 1, 0, len(a)-1)
	}
	return st
}

func (st *CountingSegTree) build(a []int, v, tl, tr int) {
	if tl == tr {
		if a[tl] == st.x {
			st.t[v] = 1
		}
		return
	}
	tm := tl + (tr-tl)/2
	st.build(a, v*2, tl, tm)
	st.build(a, v*2+1, tm+1, tr)
	st.t[v] = st.t[v*2] + st.t[v*2+1]
}

func (st *CountingSegTree) Sum(v, tl, tr, l, r int) int {
	if l > r {
		return 0
	}
	if l == tl && r == tr {
		return st.t[v]
	}
	tm := tl + (tr-tl)/2
	return st.Sum(v*2, tl, tm, l, min(r, tm)) +
		st.Sum(v*2+1, tm+1, tr, max(l, tm+1), r)
}

func (st *CountingSegTree) Update(v, tl, tr, pos, val int) {
	if tl == tr {
		st.t[v] = val
		return
	}
	tm := tl + (tr-tl)/2
	if pos <= tm {
		st.Update(v*2, tl, tm, pos, val)
	} else {
		st.Update(v*2+1, tm+1, tr, pos, val)
	}
	st.t[v] = st.t[v*2] + st.t[v*2+1]
}

// returns the index of k-th 'x', or -1 if it doesn't exist
func (st *CountingSegTree) FindKth(v, tl, tr, k int) int {
	if k > st.t[v] {
		return -1
	}
	if tl == tr {
		return tl
	}
	tm := tl + (tr-tl)/2
	if st.t[v*2] >= k {
		return st.FindKth(v*2, tl, tm, k)
	}
	return st.FindKth(v*2+1, tm+1, tr, k-st.t[v*2])
}
