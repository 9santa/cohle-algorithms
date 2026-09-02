package segtree

type GCDLCM struct {
	GCD int64
	LCM int64
}

func gcd(a, b int64) int64 {
	if b == 0 {
		return a
	}
	return gcd(b, a%b)
}

func lcm(a, b int64) int64 {
	return a / gcd(a, b) * b
}

func cmbGCDLCM(a, b GCDLCM) GCDLCM {
	return GCDLCM{
		GCD: gcd(a.GCD, b.GCD),
		LCM: lcm(a.LCM, b.LCM),
	}
}

type GCDLCMSegTree struct {
	t []GCDLCM
	n int
}

func NewGCDLCMSegTree(a []int) *GCDLCMSegTree {
	st := &GCDLCMSegTree{
		t: make([]GCDLCM, 4*max(1, len(a))),
		n: len(a),
	}
	if len(a) > 0 {
		st.build(a, 1, 0, len(a)-1)
	}
	return st
}

func (st *GCDLCMSegTree) build(a []int, v, tl, tr int) {
	if tl == tr {
		st.t[v] = GCDLCM{GCD: int64(a[tl]), LCM: int64(a[tl])}
		return
	}
	tm := tl + (tr-tl)/2
	st.build(a, v*2, tl, tm)
	st.build(a, v*2+1, tm+1, tr)
	st.t[v] = cmbGCDLCM(st.t[v*2], st.t[v*2+1])
}

func (st *GCDLCMSegTree) GetGCDLCM(v, tl, tr, l, r int) GCDLCM {
	if l > r {
		return GCDLCM{GCD: 0, LCM: 1} // identity {gcd, lcm}
	}
	if l == tl && r == tr {
		return st.t[v]
	}
	tm := tl + (tr-tl)/2
	return cmbGCDLCM(
		st.GetGCDLCM(v*2, tl, tm, l, min(r, tm)),
		st.GetGCDLCM(v*2+1, tm+1, tr, max(l, tm+1), r),
	)
}

func (st *GCDLCMSegTree) Update(v, tl, tr, pos, val int) {
	if tl == tr {
		st.t[v] = GCDLCM{GCD: int64(val), LCM: int64(val)}
		return
	}
	tm := tl + (tr-tl)/2
	if pos <= tm {
		st.Update(v*2, tl, tm, pos, val)
	} else {
		st.Update(v*2+1, tm+1, tr, pos, val)
	}
	st.t[v] = cmbGCDLCM(st.t[v*2], st.t[v*2+1])
}
