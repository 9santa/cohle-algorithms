package treap

// Sequence treap (indices 0..n-1), Monoid
// point set/get, insert/erase, range prod, max_right, min_left
type ImplicitTreap[T any] struct {
	root *implicitTreapNode[T]
	op   func(T, T) T
	id   func() T
	rng  splitmix64
}

type implicitTreapNode[T any] struct {
	x    T
	prod T
	size int
	pri  uint32
	l    *implicitTreapNode[T]
	r    *implicitTreapNode[T]
}

func NewImplicitTreap[T any](op func(T, T) T, id func() T) *ImplicitTreap[T] {
	return &ImplicitTreap[T]{op: op, id: id, rng: newSplitMix64()}
}

func NewImplicitTreapFromSlice[T any](a []T, op func(T, T) T, id func() T) *ImplicitTreap[T] {
	t := NewImplicitTreap(op, id)
	t.BuildFromSlice(a)
	return t
}

func (t *ImplicitTreap[T]) Reset() {
	t.root = nil
}

// O(n)
func (t *ImplicitTreap[T]) Build(n int, f func(int) T) {
	a := make([]T, n)
	for i := 0; i < n; i++ {
		a[i] = f(i)
	}
	t.BuildFromSlice(a)
}

func (t *ImplicitTreap[T]) BuildWithIdentity(n int) {
	t.Build(n, func(int) T { return t.id() })
}

func (t *ImplicitTreap[T]) BuildFromSlice(a []T) {
	t.Reset()
	t.root = t.buildCartesian(a)
}

func (t *ImplicitTreap[T]) Size() int { return t.size(t.root) }

// O(log n)
func (t *ImplicitTreap[T]) Get(pos int) T {
	if !(0 <= pos && pos < t.Size()) {
		panic("Get: invalid index")
	}
	a, b := t.split(t.root, pos)
	c, d := t.split(b, 1)
	res := c.x
	t.root = t.merge(a, t.merge(c, d))
	return res
}

// O(log n)
func (t *ImplicitTreap[T]) Set(pos int, val T) {
	if !(0 <= pos && pos < t.Size()) {
		panic("Set: invalid index")
	}
	a, b := t.split(t.root, pos)
	c, d := t.split(b, 1)
	c.x = val
	t.pull(c)
	t.root = t.merge(a, t.merge(c, d))
}

// O(log n)
func (t *ImplicitTreap[T]) Insert(pos int, x T) {
	if !(0 <= pos && pos <= t.Size()) {
		panic("Insert: invalid index")
	}
	a, b := t.split(t.root, pos)
	nd := t.newNode(x)
	t.root = t.merge(t.merge(a, nd), b)
}

// O(log n)
func (t *ImplicitTreap[T]) Erase(pos int) {
	if !(0 <= pos && pos < t.Size()) {
		panic("Erase: invalid index")
	}
	a, b := t.split(t.root, pos)
	_, d := t.split(b, 1)
	t.root = t.merge(a, d)
}

// [l, r), O(log n)
func (t *ImplicitTreap[T]) Prod(l, r int) T {
	if !(0 <= l && l <= r && r <= t.Size()) {
		panic("Prod: invalid range")
	}
	a, b := t.split(t.root, l)
	c, d := t.split(b, r-l)
	res := t.prodAll(c)
	t.root = t.merge(a, t.merge(c, d))
	return res
}

func (t *ImplicitTreap[T]) ProdAll() T { return t.prodAll(t.root) }

// O(log n)
// check(id) = true and monotone under appending
func (t *ImplicitTreap[T]) MaxRight(check func(T) bool, l int) int {
	if !(0 <= l && l <= t.Size()) {
		panic("MaxRight: invalid index")
	}
	if !check(t.id()) {
		panic("MaxRight: check(id) must be true")
	}
	a, b := t.split(t.root, l)
	sm := t.id()
	length := t.maxRightDFS(b, &sm, check)
	t.root = t.merge(a, b)
	return l + length
}

// O(log n)
// check(id) = true and monotone under appending
func (t *ImplicitTreap[T]) MinLeft(check func(T) bool, r int) int {
	if !(0 <= r && r <= t.Size()) {
		panic("MinLeft: invalid index")
	}
	if !check(t.id()) {
		panic("MinLeft: check(id) must be true")
	}
	a, b := t.split(t.root, r)
	sm := t.id()
	length := t.minLeftDFS(a, &sm, check)
	t.root = t.merge(a, b)
	return r - length
}

// O(n)
func (t *ImplicitTreap[T]) GetAll() []T {
	out := make([]T, 0, t.Size())
	t.inorder(t.root, &out)
	return out
}

func (t *ImplicitTreap[T]) newNode(x T) *implicitTreapNode[T] {
	return &implicitTreapNode[T]{x: x, prod: x, size: 1, pri: t.rng.Next()}
}

func (t *ImplicitTreap[T]) size(n *implicitTreapNode[T]) int {
	if n == nil {
		return 0
	}
	return n.size
}

func (t *ImplicitTreap[T]) prodAll(n *implicitTreapNode[T]) T {
	if n == nil {
		return t.id()
	}
	return n.prod
}

func (t *ImplicitTreap[T]) pull(n *implicitTreapNode[T]) {
	if n == nil {
		return
	}
	n.size = 1 + t.size(n.l) + t.size(n.r)
	n.prod = t.op(t.op(t.prodAll(n.l), n.x), t.prodAll(n.r))
}

func (t *ImplicitTreap[T]) split(n *implicitTreapNode[T], k int) (*implicitTreapNode[T], *implicitTreapNode[T]) {
	if n == nil {
		return nil, nil
	}
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

func (t *ImplicitTreap[T]) merge(a, b *implicitTreapNode[T]) *implicitTreapNode[T] {
	if a == nil {
		return b
	}
	if b == nil {
		return a
	}
	if a.pri < b.pri {
		a.r = t.merge(a.r, b)
		t.pull(a)
		return a
	}
	b.l = t.merge(a, b.l)
	t.pull(b)
	return b
}

func (t *ImplicitTreap[T]) buildCartesian(a []T) *implicitTreapNode[T] {
	if len(a) == 0 {
		return nil
	}
	st := make([]*implicitTreapNode[T], 0, len(a))
	for i := 0; i < len(a); i++ {
		cur := t.newNode(a[i])
		var last *implicitTreapNode[T]
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
	var dfs func(*implicitTreapNode[T])
	dfs = func(n *implicitTreapNode[T]) {
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

func (t *ImplicitTreap[T]) maxRightDFS(n *implicitTreapNode[T], sm *T, check func(T) bool) int {
	if n == nil {
		return 0
	}
	candL := t.op(*sm, t.prodAll(n.l))
	if check(candL) {
		*sm = candL
		candM := t.op(*sm, n.x)
		if check(candM) {
			*sm = candM
			return t.size(n.l) + 1 + t.maxRightDFS(n.r, sm, check)
		}
		return t.size(n.l)
	}
	return t.maxRightDFS(n.l, sm, check)
}

func (t *ImplicitTreap[T]) minLeftDFS(n *implicitTreapNode[T], sm *T, check func(T) bool) int {
	if n == nil {
		return 0
	}
	candR := t.op(t.prodAll(n.r), *sm)
	if check(candR) {
		*sm = candR
		candM := t.op(n.x, *sm)
		if check(candM) {
			*sm = candM
			return t.size(n.r) + 1 + t.minLeftDFS(n.l, sm, check)
		}
		return t.size(n.r)
	}
	return t.minLeftDFS(n.r, sm, check)
}

func (t *ImplicitTreap[T]) inorder(n *implicitTreapNode[T], out *[]T) {
	if n == nil {
		return
	}
	t.inorder(n.l, out)
	*out = append(*out, n.x)
	t.inorder(n.r, out)
}
