package segtree

type XorBasis60 struct {
	Basis [60]int64
}

func (x *XorBasis60) Insert(v int64) {
	cur := v
	for b := 59; b >= 0; b-- {
		if (cur>>b)&1 == 0 {
			continue
		}
		if x.Basis[b] == 0 {
			x.Basis[b] = cur
			return
		}
		cur ^= x.Basis[b]
	}
}

func (x *XorBasis60) MergeBasis(other XorBasis60) {
	for i := 59; i >= 0; i-- {
		if other.Basis[i] != 0 {
			x.Insert(other.Basis[i])
		}
	}
}

type XorBasisSegTree struct {
	n int
	t []XorBasis60
}

func NewXorBasisSegTree(a []int64) *XorBasisSegTree {
	st := &XorBasisSegTree{
		n: len(a),
		t: make([]XorBasis60, 2*max(1, len(a))),
	}
	for i := 0; i < len(a); i++ {
		st.t[st.n+i].Insert(a[i])
	}
	for i := st.n - 1; i > 0; i-- {
		st.t[i] = st.t[i<<1]
		st.t[i].MergeBasis(st.t[i<<1|1])
	}
	return st
}

// [l, r)
func (st *XorBasisSegTree) Query(l, r int) XorBasis60 {
	var res XorBasis60
	for l, r = l+st.n, r+st.n; l < r; l, r = l>>1, r>>1 {
		if l&1 == 1 {
			res.MergeBasis(st.t[l])
			l++
		}
		if r&1 == 1 {
			r--
			res.MergeBasis(st.t[r])
		}
	}
	return res
}
