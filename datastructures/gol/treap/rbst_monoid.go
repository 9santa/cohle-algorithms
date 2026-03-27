package treap

// Randomized Binary Search Tree
// sequence, random merge + Monoid + optional persistence + reverse
// merge(a, b) chooses root from a with prob |a| / (|a| + |b|) else from b
type RBSTSeqMonoid[T any] struct {
	op  func(T, T) T
	id  func() T
	rng xorShift128
}

type rbstSeqMonoidNode[T any] struct {
	l       *rbstSeqMonoidNode[T]
	r       *rbstSeqMonoidNode[T]
	x       T
	prod    T
	revProd T
	size    int
	rev     bool
}

func NewRBSTSeqMonoid[T any](op func(T, T) T, id func() T) *RBSTSeqMonoid[T] {
	return &RBSTSeqMonoid[T]{op: op, id: id, rng: newXorShift128()}
}

func (t *RBSTSeqMonoid[T]) NewNode(x T) *rbstSeqMonoidNode[T] {
	return &rbstSeqMonoidNode[T]{x: x, prod: x, revProd: x, size: 1}
}

func (t *RBSTSeqMonoid[T]) Size(n *rbstSeqMonoidNode[T]) int {
	if n == nil {
		return 0
	}
	return n.size
}

func (t *RBSTSeqMonoid[T]) ProdAll(n *rbstSeqMonoidNode[T]) T {
	if n == nil {
		return t.id()
	}
	return n.prod
}

func (t *RBSTSeqMonoid[T]) RevProdAll(n *rbstSeqMonoidNode[T]) T {
	if n == nil {
		return t.id()
	}
	return n.revProd
}

func (t *RBSTSeqMonoid[T]) applyRev(n *rbstSeqMonoidNode[T]) {
	if n == nil {
		return
	}
	n.rev = !n.rev // reverse lazy reverse flag
	n.l, n.r = n.r, n.l
	n.prod, n.revProd = n.revProd, n.prod
}

// push reverse lazy to children
func (t *RBSTSeqMonoid[T]) push(n *rbstSeqMonoidNode[T]) {
	if n == nil || !n.rev {
		return
	}
	t.applyRev(n.l)
	t.applyRev(n.r)
	n.rev = false
}

// pull one layer from children
func (t *RBSTSeqMonoid[T]) pull(n *rbstSeqMonoidNode[T]) {
	if n == nil {
		return
	}
	n.size = 1 + t.Size(n.l) + t.Size(n.r)
	n.prod = t.op(t.op(t.ProdAll(n.l), n.x), t.ProdAll(n.r))
	// reverse: right reversed + x + left reversed
	n.revProd = t.op(t.op(t.RevProdAll(n.r), n.x), t.RevProdAll(n.l))
}

// split by index: [0..k) and [k..)
func (t *RBSTSeqMonoid[T]) Split(n *rbstSeqMonoidNode[T], k int) (*rbstSeqMonoidNode[T], *rbstSeqMonoidNode[T]) {
	if n == nil {
		return nil, nil
	}
	t.push(n)
	sl := t.Size(n.l)
	if k <= sl {
		a, b := t.Split(n.l, k)
		n.l = b
		t.pull(n)
		return a, n
	}
	a, b := t.Split(n.r, k-sl-1)
	n.r = a
	t.pull(n)
	return n, b
}

// merge: random root by sizes
func (t *RBSTSeqMonoid[T]) Merge(a, b *rbstSeqMonoidNode[T]) *rbstSeqMonoidNode[T] {
	if a == nil {
		return b
	}
	if b == nil {
		return a
	}
	sa := uint32(t.Size(a))
	sb := uint32(t.Size(b))
	if t.rng.Next()%(sa+sb) < sa {
		t.push(a)
		a.r = t.Merge(a.r, b)
		t.pull(a)
		return a
	}
	t.push(b)
	b.l = t.Merge(a, b.l)
	t.pull(b)
	return b
}

// sequence ops (all expected O(log n))
func (t *RBSTSeqMonoid[T]) Insert(n *rbstSeqMonoidNode[T], pos int, x T) *rbstSeqMonoidNode[T] {
	a, b := t.Split(n, pos)
	return t.Merge(t.Merge(a, t.NewNode(x)), b)
}

func (t *RBSTSeqMonoid[T]) Erase(n *rbstSeqMonoidNode[T], pos int) *rbstSeqMonoidNode[T] {
	a, b := t.Split(n, pos)
	_, d := t.Split(b, 1)
	return t.Merge(a, d)
}

func (t *RBSTSeqMonoid[T]) Get(n *rbstSeqMonoidNode[T], pos int) T {
	a, b := t.Split(n, pos)
	c, d := t.Split(b, 1)
	res := t.id()
	if c != nil {
		res = c.x
	}
	_ = t.Merge(a, t.Merge(c, d))
	return res
}

func (t *RBSTSeqMonoid[T]) Set(n *rbstSeqMonoidNode[T], pos int, x T) *rbstSeqMonoidNode[T] {
	a, b := t.Split(n, pos)
	c, d := t.Split(b, 1)
	if c != nil {
		c.x = x
		t.pull(c)
	}
	return t.Merge(a, t.Merge(c, d))
}

// range prod on [l, r)
func (t *RBSTSeqMonoid[T]) Prod(n *rbstSeqMonoidNode[T], l, r int) T {
	a, b := t.Split(n, l)
	c, d := t.Split(b, r-l)
	res := t.ProdAll(c)
	_ = t.Merge(a, t.Merge(c, d))
	return res
}

// reverse [l, r)
func (t *RBSTSeqMonoid[T]) Reverse(n *rbstSeqMonoidNode[T], l, r int) *rbstSeqMonoidNode[T] {
	a, b := t.Split(n, l)
	c, d := t.Split(b, r-l)
	t.applyRev(c)
	return t.Merge(a, t.Merge(c, d))
}
