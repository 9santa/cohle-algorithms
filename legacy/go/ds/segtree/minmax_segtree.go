package segtree

import "math"

type MaxCount struct {
	Max   int
	Count int
}

// pair {max, occurrences}
func cmbMaxCount(a, b MaxCount) MaxCount {
	if a.Max > b.Max {
		return a
	}
	if b.Max > a.Max {
		return b
	}
	return MaxCount{Max: a.Max, Count: a.Count + b.Count}
}

type MinMaxSegTree struct {
	t []MaxCount
	n int
}

func NewMinMaxSegTree(a []int) *MinMaxSegTree {
	st := &MinMaxSegTree{
		t: make([]MaxCount, 4*max(1, len(a))),
		n: len(a),
	}
	if len(a) > 0 {
		st.build(a, 1, 0, len(a)-1)
	}
	return st
}

func (st *MinMaxSegTree) build(a []int, v, tl, tr int) {
	if tl == tr {
		st.t[v] = MaxCount{Max: a[tl], Count: 1}
		return
	}
	tm := tl + (tr-tl)/2
	st.build(a, v*2, tl, tm)
	st.build(a, v*2+1, tm+1, tr)
	st.t[v] = cmbMaxCount(st.t[v*2], st.t[v*2+1])
}

func (st *MinMaxSegTree) GetMax(v, tl, tr, l, r int) MaxCount {
	if l > r {
		return MaxCount{Max: math.MinInt, Count: 0}
	}
	if l == tl && r == tr {
		return st.t[v]
	}
	tm := tl + (tr-tl)/2
	return cmbMaxCount(
		st.GetMax(v*2, tl, tm, l, min(r, tm)),
		st.GetMax(v*2+1, tm+1, tr, max(l, tm+1), r),
	)
}

func (st *MinMaxSegTree) Update(v, tl, tr, pos, val int) {
	if tl == tr {
		st.t[v] = MaxCount{Max: val, Count: 1}
		return
	}
	tm := tl + (tr-tl)/2
	if pos <= tm {
		st.Update(v*2, tl, tm, pos, val)
	} else {
		st.Update(v*2+1, tm+1, tr, pos, val)
	}
	st.t[v] = cmbMaxCount(st.t[v*2], st.t[v*2+1])
}
