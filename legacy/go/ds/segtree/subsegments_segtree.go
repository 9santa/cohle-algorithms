package segtree

type SubsegmentData struct {
	Sum  int
	Pref int
	Suf  int
	Ans  int
}

func makeSubsegmentData(val int) SubsegmentData {
	best := max(0, val)
	return SubsegmentData{
		Sum:  val,
		Pref: best,
		Suf:  best,
		Ans:  best,
	}
}

func cmbSubsegment(l, r SubsegmentData) SubsegmentData {
	return SubsegmentData{
		Sum:  l.Sum + r.Sum,
		Pref: max(l.Pref, l.Sum+r.Pref),
		Suf:  max(r.Suf, r.Sum+l.Suf),
		Ans:  max(max(l.Ans, r.Ans), l.Suf+r.Pref),
	}
}

type SubsegmentsSegTree struct {
	t []SubsegmentData
	n int
}

func NewSubsegmentsSegTree(a []int) *SubsegmentsSegTree {
	st := &SubsegmentsSegTree{
		t: make([]SubsegmentData, 4*max(1, len(a))),
		n: len(a),
	}
	if len(a) > 0 {
		st.build(a, 1, 0, len(a)-1)
	}
	return st
}

func (st *SubsegmentsSegTree) build(a []int, v, tl, tr int) {
	if tl == tr {
		st.t[v] = makeSubsegmentData(a[tl])
		return
	}
	tm := tl + (tr-tl)/2
	st.build(a, v*2, tl, tm)
	st.build(a, v*2+1, tm+1, tr)
	st.t[v] = cmbSubsegment(st.t[v*2], st.t[v*2+1])
}

func (st *SubsegmentsSegTree) Update(v, tl, tr, pos, val int) {
	if tl == tr {
		st.t[v] = makeSubsegmentData(val)
		return
	}
	tm := tl + (tr-tl)/2
	if pos <= tm {
		st.Update(v*2, tl, tm, pos, val)
	} else {
		st.Update(v*2+1, tm+1, tr, pos, val)
	}
	st.t[v] = cmbSubsegment(st.t[v*2], st.t[v*2+1])
}

func (st *SubsegmentsSegTree) Query(v, tl, tr, l, r int) SubsegmentData {
	if l > r {
		return makeSubsegmentData(0)
	}
	if l == tl && r == tr {
		return st.t[v]
	}
	tm := tl + (tr-tl)/2
	return cmbSubsegment(
		st.Query(v*2, tl, tm, l, min(r, tm)),
		st.Query(v*2+1, tm+1, tr, max(l, tm+1), r),
	)
}
