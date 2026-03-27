package treap

// Lazy implicit treap with Acted Monoid
type LazyImplicitTreap[X any, A comparable] struct {
	root *lazyImplicitTreapNode[X, A]
	opX  func(X, X) X
	idX  func() X
	opA  func(A, A) A
	idA  func() A
	act  func(X, A, int) X
	rng  splitmix64
}

type lazyImplicitTreapNode[X any, A comparable] struct {
	x    X
	prod X
	size int
	pri  uint32
	l    *lazyImplicitTreapNode[X, A]
	r    *lazyImplicitTreapNode[X, A]
	lazy A
}

func NewLazyImplicitTreap[X any, A comparable](
	opX func(X, X) X,
	idX func() X,
	opA func(A, A) A,
	idA func() A,
	act func(X, A, int) X,
) *LazyImplicitTreap[X, A] {
	return &LazyImplicitTreap[X, A]{
		opX: opX,
		idX: idX,
		opA: opA,
		idA: idA,
		act: act,
		rng: newSplitMix64(),
	}
}

func NewLazyImplicitTreapFromSlice[X any, A comparable](
	a []X,
	opX func(X, X) X,
	idX func() X,
	opA func(A, A) A,
	idA func() A,
	act func(X, A, int) X,
) *LazyImplicitTreap[X, A] {
	t := NewLazyImplicitTreap(opX, idX, opA, idA, act)
	t.BuildFromSlice(a)
	return t
}

func (t *LazyImplicitTreap[X, A]) Reset() {
	t.root = nil
}

func (t *LazyImplicitTreap[X, A]) Build(n int, f func(int) X) {
	a := make([]X, n)
	for i := 0; i < n; i++ {
		a[i] = f(i)
	}
	t.BuildFromSlice(a)
}

func (t *LazyImplicitTreap[X, A]) BuildWithIdentity(n int) {
	t.Build(n, func(int) X { return t.idX() })
}

func (t *LazyImplicitTreap[X, A]) BuildFromSlice(a []X) {
	t.Reset()
	t.root = t.buildCartesian(a)
}

func (t *LazyImplicitTreap[X, A]) Size() int { return t.size(t.root) }

func (t *LazyImplicitTreap[X, A]) Get(pos int) X {
	if !(0 <= pos && pos < t.Size()) {
		panic("Get: invalid index")
	}
	a, b := t.split(t.root, pos)
	c, d := t.split(b, 1)
	res := c.x
	t.root = t.merge(a, t.merge(c, d))
	return res
}

func (t *LazyImplicitTreap[X, A]) Set(pos int, val X) {
	if !(0 <= pos && pos < t.Size()) {
		panic("Set: invalid index")
	}
	a, b := t.split(t.root, pos)
	c, d := t.split(b, 1)
	c.x = val
	t.pull(c)
	t.root = t.merge(a, t.merge(c, d))
}

func (t *LazyImplicitTreap[X, A]) Insert(pos int, x X) {
	if !(0 <= pos && pos <= t.Size()) {
		panic("Insert: invalid index")
	}
	a, b := t.split(t.root, pos)
	nd := t.newNode(x)
	t.root = t.merge(t.merge(a, nd), b)
}

func (t *LazyImplicitTreap[X, A]) Erase(pos int) {
	if !(0 <= pos && pos < t.Size()) {
		panic("Erase: invalid index")
	}
	a, b := t.split(t.root, pos)
	_, d := t.split(b, 1)
	t.root = t.merge(a, d)
}

// [l, r)
func (t *LazyImplicitTreap[X, A]) Prod(l, r int) X {
	if !(0 <= l && l <= r && r <= t.Size()) {
		panic("Prod: invalid range")
	}
	a, b := t.split(t.root, l)
	c, d := t.split(b, r-l)
	res := t.prodAll(c)
	t.root = t.merge(a, t.merge(c, d))
	return res
}

func (t *LazyImplicitTreap[X, A]) ProdAll() X { return t.prodAll(t.root) }

// apply action 'a' on [l, r)
func (t *LazyImplicitTreap[X, A]) Apply(l, r int, a A) {
	if !(0 <= l && l <= r && r <= t.Size()) {
		panic("Apply: invalid range")
	}
	p, q := t.split(t.root, l)
	m, s := t.split(q, r-l)
	t.applyAt(m, a)
	t.root = t.merge(p, t.merge(m, s))
}

func (t *LazyImplicitTreap[X, A]) MaxRight(check func(X) bool, l int) int {
	if !(0 <= l && l <= t.Size()) {
		panic("MaxRight: invalid index")
	}
	if !check(t.idX()) {
		panic("MaxRight: check(id) must be true")
	}
	a, b := t.split(t.root, l)
	sm := t.idX()
	length := t.maxRightDFS(b, &sm, check)
	t.root = t.merge(a, b)
	return l + length
}

func (t *LazyImplicitTreap[X, A]) MinLeft(check func(X) bool, r int) int {
	if !(0 <= r && r <= t.Size()) {
		panic("MinLeft: invalid index")
	}
	if !check(t.idX()) {
		panic("MinLeft: check(id) must be true")
	}
	a, b := t.split(t.root, r)
	sm := t.idX()
	length := t.minLeftDFS(a, &sm, check)
	t.root = t.merge(a, b)
	return r - length
}

func (t *LazyImplicitTreap[X, A]) GetAll() []X {
	out := make([]X, 0, t.Size())
	t.inorder(t.root, &out)
	return out
}

func (t *LazyImplicitTreap[X, A]) newNode(x X) *lazyImplicitTreapNode[X, A] {
	return &lazyImplicitTreapNode[X, A]{
		x:    x,
		prod: x,
		size: 1,
		pri:  t.rng.Next(),
		lazy: t.idA(),
	}
}

func (t *LazyImplicitTreap[X, A]) size(n *lazyImplicitTreapNode[X, A]) int {
	if n == nil {
		return 0
	}
	return n.size
}

func (t *LazyImplicitTreap[X, A]) prodAll(n *lazyImplicitTreapNode[X, A]) X {
	if n == nil {
		return t.idX()
	}
	return n.prod
}

func (t *LazyImplicitTreap[X, A]) pull(n *lazyImplicitTreapNode[X, A]) {
	if n == nil {
		return
	}
	n.size = 1 + t.size(n.l) + t.size(n.r)
	n.prod = t.opX(t.opX(t.prodAll(n.l), n.x), t.prodAll(n.r))
}

func (t *LazyImplicitTreap[X, A]) applyAt(n *lazyImplicitTreapNode[X, A], a A) {
	if n == nil {
		return
	}
	n.x = t.act(n.x, a, 1)
	n.prod = t.act(n.prod, a, n.size)
	n.lazy = t.opA(n.lazy, a)
}

func (t *LazyImplicitTreap[X, A]) push(n *lazyImplicitTreapNode[X, A]) {
	if n == nil || n.lazy == t.idA() {
		return
	}
	t.applyAt(n.l, n.lazy)
	t.applyAt(n.r, n.lazy)
	n.lazy = t.idA()
}

func (t *LazyImplicitTreap[X, A]) split(n *lazyImplicitTreapNode[X, A], k int) (*lazyImplicitTreapNode[X, A], *lazyImplicitTreapNode[X, A]) {
	if n == nil {
		return nil, nil
	}
	t.push(n)
	if t.size(n.l) >= k {
		a, b := t.split(n.l, k)
		n.l = b
		t.pull(n)
		return a, n
	}
	a, b := t.split(n.r, k-t.size(n.l)-1)
	n.r = a
	t.pull(n)
	return n, b
}

func (t *LazyImplicitTreap[X, A]) merge(a, b *lazyImplicitTreapNode[X, A]) *lazyImplicitTreapNode[X, A] {
	if a == nil {
		return b
	}
	if b == nil {
		return a
	}
	if a.pri < b.pri {
		t.push(a)
		a.r = t.merge(a.r, b)
		t.pull(a)
		return a
	}
	t.push(b)
	b.l = t.merge(a, b.l)
	t.pull(b)
	return b
}

func (t *LazyImplicitTreap[X, A]) buildCartesian(a []X) *lazyImplicitTreapNode[X, A] {
	if len(a) == 0 {
		return nil
	}
	st := make([]*lazyImplicitTreapNode[X, A], 0, len(a))
	for i := 0; i < len(a); i++ {
		cur := t.newNode(a[i])
		var last *lazyImplicitTreapNode[X, A]
		for len(st) > 0 && st[len(st)-1].pri > cur.pri {
			last = st[len(st)-1]
			st = st[:len(st)-1]
		}
		cur.l = last
		if len(st) > 0 {
			st[len(st)-1].r = cur
		}
		st = append(st, cur)
	}
	rt := st[0]
	var dfs func(*lazyImplicitTreapNode[X, A])
	dfs = func(n *lazyImplicitTreapNode[X, A]) {
		if n == nil {
			return
		}
		dfs(n.l)
		dfs(n.r)
		t.pull(n)
	}
	dfs(rt)
	return rt
}

func (t *LazyImplicitTreap[X, A]) maxRightDFS(n *lazyImplicitTreapNode[X, A], sm *X, check func(X) bool) int {
	if n == nil {
		return 0
	}
	t.push(n)
	candL := t.opX(*sm, t.prodAll(n.l))
	if check(candL) {
		*sm = candL
		candM := t.opX(*sm, n.x)
		if check(candM) {
			*sm = candM
			return t.size(n.l) + 1 + t.maxRightDFS(n.r, sm, check)
		}
		return t.size(n.l)
	}
	return t.maxRightDFS(n.l, sm, check)
}

func (t *LazyImplicitTreap[X, A]) minLeftDFS(n *lazyImplicitTreapNode[X, A], sm *X, check func(X) bool) int {
	if n == nil {
		return 0
	}
	t.push(n)
	candR := t.opX(t.prodAll(n.r), *sm)
	if check(candR) {
		*sm = candR
		candM := t.opX(n.x, *sm)
		if check(candM) {
			*sm = candM
			return t.size(n.r) + 1 + t.minLeftDFS(n.l, sm, check)
		}
		return t.size(n.r)
	}
	return t.minLeftDFS(n.r, sm, check)
}

func (t *LazyImplicitTreap[X, A]) inorder(n *lazyImplicitTreapNode[X, A], out *[]X) {
	if n == nil {
		return
	}
	t.push(n)
	t.inorder(n.l, out)
	*out = append(*out, n.x)
	t.inorder(n.r, out)
}
