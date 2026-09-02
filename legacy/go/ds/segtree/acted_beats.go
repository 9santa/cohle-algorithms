package segtree

type BeatsSegtree[X any, A any] struct {
	n    int
	log  int
	size int // n = array size, size = actual segtree size

	data []X
	lazy []A

	opX     func(X, X) X
	idX     func() X
	opA     func(A, A) A
	idA     func() A
	eqA     func(A, A) bool
	act     func(X, A, int64) X
	hasFail func(X) bool
}

func NewBeatsSegtree[X any, A any](
	n int,
	opX func(X, X) X,
	idX func() X,
	opA func(A, A) A,
	idA func() A,
	eqA func(A, A) bool,
	act func(X, A, int64) X,
	hasFail func(X) bool,
) *BeatsSegtree[X, A] {
	st := &BeatsSegtree[X, A]{
		opX:     opX,
		idX:     idX,
		opA:     opA,
		idA:     idA,
		eqA:     eqA,
		act:     act,
		hasFail: hasFail,
	}
	st.Build(n, func(int) X { return idX() })
	return st
}

func NewBeatsSegtreeFromSlice[X any, A any](
	v []X,
	opX func(X, X) X,
	idX func() X,
	opA func(A, A) A,
	idA func() A,
	eqA func(A, A) bool,
	act func(X, A, int64) X,
	hasFail func(X) bool,
) *BeatsSegtree[X, A] {
	st := &BeatsSegtree[X, A]{
		opX:     opX,
		idX:     idX,
		opA:     opA,
		idA:     idA,
		eqA:     eqA,
		act:     act,
		hasFail: hasFail,
	}
	st.Build(len(v), func(i int) X { return v[i] })
	return st
}

// build with a custom function F
func (st *BeatsSegtree[X, A]) Build(m int, f func(int) X) {
	st.n = m
	st.log = 1
	for (1 << st.log) < st.n {
		st.log++
	}
	st.size = 1 << st.log
	st.data = make([]X, st.size<<1)
	for i := range st.data {
		st.data[i] = st.idX()
	}
	st.lazy = make([]A, st.size)
	for i := range st.lazy {
		st.lazy[i] = st.idA()
	}
	for i := 0; i < st.n; i++ {
		st.data[st.size+i] = f(i)
	}
	for i := st.size - 1; i >= 1; i-- {
		st.update(i)
	}
}

func (st *BeatsSegtree[X, A]) update(p int) {
	st.data[p] = st.opX(st.data[p<<1], st.data[p<<1|1])
}

// set leaf p = x, push stored lazy before assignment, update parents after assignment
func (st *BeatsSegtree[X, A]) Set(p int, x X) {
	if !(0 <= p && p < st.n) {
		panic("Set: invalid index")
	}
	p += st.size
	for i := st.log; i >= 1; i-- {
		st.push(p >> i)
	}
	st.data[p] = x
	for i := 1; i <= st.log; i++ {
		st.update(p >> i)
	}
}

// if MX = multiplicative Monoid
func (st *BeatsSegtree[X, A]) Multiply(p int, x X) {
	if !(0 <= p && p < st.n) {
		panic("Multiply: invalid index")
	}
	p += st.size
	for i := st.log; i >= 1; i-- {
		st.push(p >> i)
	}
	st.data[p] = st.opX(st.data[p], x)
	for i := 1; i <= st.log; i++ {
		st.update(p >> i)
	}
}

// get value at index p, push stored lazy if it exists before returning the value
func (st *BeatsSegtree[X, A]) Get(p int) X {
	if !(0 <= p && p < st.n) {
		panic("Get: invalid index")
	}
	p += st.size
	for i := st.log; i >= 1; i-- {
		st.push(p >> i)
	}
	return st.data[p]
}

func (st *BeatsSegtree[X, A]) GetAll() []X {
	for i := 1; i < st.size; i++ {
		st.push(i)
	}
	res := make([]X, st.n)
	copy(res, st.data[st.size:st.size+st.n])
	return res
}

// [L, R) range query (Monoid::op operation)
func (st *BeatsSegtree[X, A]) Prod(l, r int) X {
	if !(0 <= l && l <= r && r <= st.n) {
		panic("Prod: invalid range")
	}
	if l == r {
		return st.idX()
	}
	l += st.size
	r += st.size
	for i := st.log; i >= 1; i-- {
		if ((l >> i) << i) != l {
			st.push(l >> i)
		}
		if ((r >> i) << i) != r {
			st.push((r - 1) >> i)
		}
	}
	xl, xr := st.idX(), st.idX()
	for l < r {
		if l&1 == 1 {
			xl = st.opX(xl, st.data[l])
			l++
		}
		if r&1 == 1 {
			r--
			xr = st.opX(st.data[r], xr)
		}
		l >>= 1
		r >>= 1
	}
	return st.opX(xl, xr)
}

// whole tree
func (st *BeatsSegtree[X, A]) ProdAll() X {
	return st.data[1]
}

func (st *BeatsSegtree[X, A]) Apply(l, r int, a A) {
	if !(0 <= l && l <= r && r <= st.n) {
		panic("Apply: invalid range")
	}
	if l == r {
		return
	}
	l += st.size
	r += st.size
	// push top-down along the two boundary root-to-leaf paths
	for i := st.log; i >= 1; i-- {
		if ((l >> i) << i) != l {
			st.push(l >> i)
		}
		if ((r >> i) << i) != r {
			st.push((r - 1) >> i)
		}
	}
	l2, r2 := l, r
	// apply the A act operation
	for l < r {
		if l&1 == 1 {
			st.applyAt(l, a)
			l++
		}
		if r&1 == 1 {
			r--
			st.applyAt(r, a)
		}
		l >>= 1
		r >>= 1
	}
	l, r = l2, r2
	// recompute affected ancestors on boundary paths (those whose children changed)
	for i := 1; i <= st.log; i++ {
		if ((l >> i) << i) != l {
			st.update(l >> i)
		}
		if ((r >> i) << i) != r {
			st.update((r - 1) >> i)
		}
	}
}

func (st *BeatsSegtree[X, A]) MaxRight(check func(X) bool, l int) int {
	if !(0 <= l && l <= st.n) {
		panic("MaxRight: invalid index")
	}
	if !check(st.idX()) {
		panic("MaxRight: check(id) must be true")
	}
	if l == st.n {
		return st.n
	}
	l += st.size
	for i := st.log; i >= 1; i-- {
		st.push(l >> i)
	}
	sm := st.idX()
	for {
		for l%2 == 0 {
			l >>= 1
		}
		if !check(st.opX(sm, st.data[l])) {
			for l < st.size {
				st.push(l)
				l <<= 1
				if check(st.opX(sm, st.data[l])) {
					sm = st.opX(sm, st.data[l])
					l++
				}
			}
			return l - st.size
		}
		sm = st.opX(sm, st.data[l])
		l++
		if l&-l == l {
			break
		}
	}
	return st.n
}

func (st *BeatsSegtree[X, A]) MinLeft(check func(X) bool, r int) int {
	if !(0 <= r && r <= st.n) {
		panic("MinLeft: invalid index")
	}
	if !check(st.idX()) {
		panic("MinLeft: check(id) must be true")
	}
	if r == 0 {
		return 0
	}
	r += st.size
	for i := st.log; i >= 1; i-- {
		st.push((r - 1) >> i)
	}
	sm := st.idX()
	for {
		r--
		for r > 1 && r%2 == 1 {
			r >>= 1
		}
		if !check(st.opX(sm, st.data[r])) {
			for r < st.size {
				st.push(r)
				r = r<<1 | 1
				if check(st.opX(sm, st.data[r])) {
					sm = st.opX(sm, st.data[r])
					r--
				}
			}
			return r + 1 - st.size
		}
		sm = st.opX(sm, st.data[r])
		if r&-r == r {
			break
		}
	}
	return 0
}

// apply act() at node p, if it's internal node -> store lazy tag
func (st *BeatsSegtree[X, A]) applyAt(p int, a A) {
	segSize := int64(1 << (st.log - topbit(p)))
	st.data[p] = st.act(st.data[p], a, segSize)
	if p < st.size {
		st.lazy[p] = st.opA(st.lazy[p], a)
		if st.hasFail(st.data[p]) {
			st.push(p)
			st.update(p)
		}
	}
}

// propagate p's pending lazy action to its children and clear it
func (st *BeatsSegtree[X, A]) push(p int) {
	if p >= len(st.lazy) || st.eqA(st.lazy[p], st.idA()) {
		return
	}
	st.applyAt(p<<1, st.lazy[p])
	st.applyAt(p<<1|1, st.lazy[p])
	st.lazy[p] = st.idA()
}
