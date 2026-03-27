package segtree

const ProductLazySegMOD int64 = 1000000007

// Multiplicative (product) lazy segment tree
type ProductLazySeg struct {
	n      int
	data   []int64
	lazy   []int64
	length []int // len of each node's segment
}

func NewProductLazySeg(arr []int64) *ProductLazySeg {
	n := 1
	for n < len(arr) {
		n <<= 1
	}
	st := &ProductLazySeg{
		n:      n,
		data:   make([]int64, 2*n),
		lazy:   make([]int64, n),
		length: make([]int, 2*n),
	}
	for i := range st.data {
		st.data[i] = 1
	}
	for i := range st.lazy {
		st.lazy[i] = 1
	}
	st.build(arr)
	return st
}

func modPowProduct(a, b int64) int64 {
	res := int64(1)
	for a %= ProductLazySegMOD; b > 0; b >>= 1 {
		if b&1 == 1 {
			res = (res * a) % ProductLazySegMOD
		}
		a = (a * a) % ProductLazySegMOD
	}
	return res
}

// Build from array
func (p *ProductLazySeg) build(arr []int64) {
	for i := range p.lazy {
		p.lazy[i] = 1 // reset lazy just in-case
	}
	// Leaves
	for i := 0; i < len(arr); i++ {
		p.data[p.n+i] = arr[i] % ProductLazySegMOD
		p.length[p.n+i] = 1
	}
	for i := p.n + len(arr); i < 2*p.n; i++ {
		p.length[i] = 1
	}
	// Internal nodes bottom-up
	for i := p.n - 1; i > 0; i-- {
		p.data[i] = (p.data[i<<1] * p.data[i<<1|1]) % ProductLazySegMOD
		p.length[i] = p.length[i<<1] + p.length[i<<1|1]
	}
}

// Apply multiplication to a node
func (p *ProductLazySeg) apply(idx int, val int64) {
	p.data[idx] = p.data[idx] * modPowProduct(val, int64(p.length[idx])) % ProductLazySegMOD
	if idx < p.n { // if not a leaf
		p.lazy[idx] = p.lazy[idx] * val % ProductLazySegMOD
	}
}

// Push lazy down one level
func (p *ProductLazySeg) push(idx int) {
	if p.lazy[idx] != 1 {
		p.apply(idx<<1, p.lazy[idx])
		p.apply(idx<<1|1, p.lazy[idx])
		p.lazy[idx] = 1
	}
}

// Push all lazy values on path from root to leaf, idx should be a leaf
func (p *ProductLazySeg) pushPath(idx int) {
	for h := topbit(p.n); h > 0; h-- {
		i := idx >> h // ancestor at height h
		if p.lazy[i] != 1 {
			p.push(i)
		}
	}
}

// Range multiplication update [l, r]
func (p *ProductLazySeg) RangeMul(l, r int, val int64) {
	val %= ProductLazySegMOD
	if val == 1 {
		return // no change
	}
	l += p.n
	r += p.n
	l0, r0 := l, r
	p.pushPath(l0)
	p.pushPath(r0)

	// Apply updates
	for l <= r {
		if l&1 == 1 {
			p.apply(l, val)
			l++
		}
		if r&1 == 0 {
			p.apply(r, val)
			r--
		}
		l >>= 1
		r >>= 1
	}

	// Update ancestors
	for l = l0; l > 0; l >>= 1 {
		if l < p.n && p.lazy[l] == 1 {
			p.data[l] = (p.data[l<<1] * p.data[l<<1|1]) % ProductLazySegMOD
		}
	}
	for r = r0; r > 0; r >>= 1 {
		if r < p.n && p.lazy[r] == 1 {
			p.data[r] = (p.data[r<<1] * p.data[r<<1|1]) % ProductLazySegMOD
		}
	}
}

// Range product query [l, r]
func (p *ProductLazySeg) RangeQuery(l, r int) int64 {
	l += p.n
	r += p.n
	p.pushPath(l)
	p.pushPath(r)
	res := int64(1)
	for l <= r {
		if l&1 == 1 {
			res = (res * p.data[l]) % ProductLazySegMOD
			l++
		}
		if r&1 == 0 {
			res = (res * p.data[r]) % ProductLazySegMOD
			r--
		}
		l >>= 1
		r >>= 1
	}
	return res
}

// Point mul update
func (p *ProductLazySeg) PointMul(idx int, val int64) {
	idx += p.n
	p.pushPath(idx)
	p.data[idx] = p.data[idx] * val % ProductLazySegMOD
	for idx >>= 1; idx > 0; idx >>= 1 {
		p.data[idx] = (p.data[idx<<1] * p.data[idx<<1|1]) % ProductLazySegMOD
	}
}

// Point query
func (p *ProductLazySeg) PointQuery(idx int) int64 {
	idx += p.n
	p.pushPath(idx)
	return p.data[idx]
}

// Recursive
type MulSegtree struct {
	n    int
	data []int64
	lazy []int64
}

func NewMulSegtree(a []int64) *MulSegtree {
	st := &MulSegtree{
		n:    len(a),
		data: make([]int64, 4*max(1, len(a))),
		lazy: make([]int64, 4*max(1, len(a))),
	}
	for i := range st.lazy {
		st.lazy[i] = 1
	}
	if len(a) > 0 {
		st.build(a, 1, 0, len(a)-1)
	}
	return st
}

func (m *MulSegtree) cmb(a, b int64) int64 {
	return (a * b) % ProductLazySegMOD
}

func (m *MulSegtree) build(a []int64, v, tl, tr int) {
	m.lazy[v] = 1
	if tl == tr {
		m.data[v] = a[tl] % ProductLazySegMOD
		return
	}
	tm := (tl + tr) >> 1
	m.build(a, v*2, tl, tm)
	m.build(a, v*2+1, tm+1, tr)
	m.data[v] = m.cmb(m.data[v*2], m.data[v*2+1])
}

func (m *MulSegtree) apply(v, l, r int, val int64) {
	m.data[v] = m.data[v] * modPowProduct(val, int64(r-l+1)) % ProductLazySegMOD
	m.lazy[v] = m.lazy[v] * val % ProductLazySegMOD
}

func (m *MulSegtree) push(v, tl, tr int) {
	if m.lazy[v] != 1 {
		mid := (tl + tr) >> 1
		m.apply(v*2, tl, mid, m.lazy[v])
		m.apply(v*2+1, mid+1, tr, m.lazy[v])
		m.lazy[v] = 1
	}
}

func (m *MulSegtree) rangeMul(v, tl, tr, l, r int, val int64) {
	if r < tl || l > tr {
		return
	}
	if l <= tl && tr <= r {
		m.apply(v, tl, tr, val)
		return
	}
	m.push(v, tl, tr)
	mid := (tl + tr) >> 1
	m.rangeMul(v*2, tl, mid, l, min(r, mid), val)
	m.rangeMul(v*2+1, mid+1, tr, max(l, mid+1), r, val)
	m.data[v] = m.cmb(m.data[v*2], m.data[v*2+1])
}

func (m *MulSegtree) rangeQuery(v, tl, tr, l, r int) int64 {
	if r < tl || l > tr {
		return 1
	}
	if l <= tl && tr <= r {
		return m.data[v]
	}
	m.push(v, tl, tr)
	tm := (tl + tr) >> 1
	left := m.rangeQuery(v*2, tl, tm, l, min(r, tm))
	right := m.rangeQuery(v*2+1, tm+1, tr, max(l, tm+1), r)
	return m.cmb(left, right)
}

func (m *MulSegtree) RangeMul(l, r int, val int64) {
	m.rangeMul(1, 0, m.n-1, l, r, val)
}

func (m *MulSegtree) RangeQuery(l, r int) int64 {
	return m.rangeQuery(1, 0, m.n-1, l, r)
}
