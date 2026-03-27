package segtree

type PST[T any] struct {
	n  int
	st []PSTNode[T] // node pool, st[0] = null node

	op func(T, T) T
	id func() T
}

type PSTNode[T any] struct {
	l   int
	r   int
	val T
}

func NewPST[T any](n int, reserveNodes int, op func(T, T) T, id func() T) *PST[T] {
	p := &PST[T]{op: op, id: id}
	p.Init(n, reserveNodes)
	return p
}

func (p *PST[T]) Init(n int, reserveNodes int) {
	p.n = n
	p.st = make([]PSTNode[T], 0, max(1, reserveNodes))
	p.st = append(p.st, PSTNode[T]{val: p.id()})
}

func (p *PST[T]) clone(x PSTNode[T]) int {
	p.st = append(p.st, x)
	return len(p.st) - 1
}

// Build from function f(i) for i in [0..n)
func (p *PST[T]) Build(f func(int) T) int {
	return p.build(0, p.n, f)
}

func (p *PST[T]) build(tl, tr int, f func(int) T) int {
	cur := PSTNode[T]{val: p.id()}
	if tr-tl == 1 {
		cur.val = f(tl)
		return p.clone(cur)
	}
	tm := (tl + tr) >> 1
	cur.l = p.build(tl, tm, f)
	cur.r = p.build(tm, tr, f)
	cur.val = p.op(p.st[cur.l].val, p.st[cur.r].val)
	return p.clone(cur)
}

// Point set: a[pos] = x
func (p *PST[T]) Set(root, pos int, x T) int {
	return p.set(root, 0, p.n, pos, x)
}

func (p *PST[T]) set(v, tl, tr, pos int, x T) int {
	cur := p.st[v]
	if tr-tl == 1 {
		cur.val = x
		return p.clone(cur)
	}
	tm := (tl + tr) >> 1
	if pos < tm {
		cur.l = p.set(cur.l, tl, tm, pos, x)
	} else {
		cur.r = p.set(cur.r, tm, tr, pos, x)
	}
	cur.val = p.op(p.st[cur.l].val, p.st[cur.r].val)
	return p.clone(cur)
}

// Point apply: a[pos] = op(a[pos], delta)
func (p *PST[T]) Apply(root, pos int, delta T) int {
	return p.apply(root, 0, p.n, pos, delta)
}

func (p *PST[T]) apply(v, tl, tr, pos int, delta T) int {
	cur := p.st[v]
	if tr-tl == 1 {
		cur.val = p.op(cur.val, delta)
		return p.clone(cur)
	}
	tm := (tl + tr) >> 1
	if pos < tm {
		cur.l = p.apply(cur.l, tl, tm, pos, delta)
	} else {
		cur.r = p.apply(cur.r, tm, tr, pos, delta)
	}
	cur.val = p.op(p.st[cur.l].val, p.st[cur.r].val)
	return p.clone(cur)
}

// Range query [l, r)
func (p *PST[T]) Prod(root, l, r int) T {
	return p.prod(root, 0, p.n, l, r)
}

func (p *PST[T]) prod(v, tl, tr, l, r int) T {
	if v == 0 || r <= tl || tr <= l {
		return p.id()
	}
	if l <= tl && tr <= r {
		return p.st[v].val
	}
	tm := (tl + tr) >> 1
	return p.op(p.prod(p.st[v].l, tl, tm, l, r), p.prod(p.st[v].r, tm, tr, l, r))
}

func (p *PST[T]) AllProd(root int) T {
	return p.st[root].val
}

// Find max r in [l..n] such that f(prod(root, l, r)) == true
// Requires: f(id()) == true
// 0-indexed, returns r (half-open end)
func (p *PST[T]) MaxRight(root, l int, f func(T) bool) int {
	if !(0 <= l && l <= p.n) {
		panic("MaxRight: invalid index")
	}
	if !f(p.id()) {
		panic("MaxRight: f(id) must be true")
	}
	if l == p.n {
		return p.n
	}
	acc := p.id()
	return p.maxRightDFS(root, 0, p.n, l, f, &acc)
}

func (p *PST[T]) maxRightDFS(v, tl, tr, l int, f func(T) bool, acc *T) int {
	if tr <= l {
		return tr // completely before l, skip
	}
	if v == 0 {
		// null node contributes id, so we can treat this whole segment as accepted
		return tr
	}
	if l <= tl {
		nxt := p.op(*acc, p.st[v].val)
		if f(nxt) {
			*acc = nxt
			return tr // take whole segment
		}
		if tr-tl == 1 {
			return tl // cannot take this single element if a leaf
		}
	}
	tm := (tl + tr) >> 1
	res := p.maxRightDFS(p.st[v].l, tl, tm, l, f, acc) // try to take left child
	if res < tm {
		return res // if couldn't take the whole left child segment, answer inside left
	}
	return p.maxRightDFS(p.st[v].r, tm, tr, l, f, acc) // try to extend via right child, if took the whole left child
}

func (p *PST[T]) MinLeft(root, r int, f func(T) bool) int {
	if !(0 <= r && r <= p.n) {
		panic("MinLeft: invalid index")
	}
	if !f(p.id()) {
		panic("MinLeft: f(id) must be true")
	}
	if r == 0 {
		return 0
	}
	acc := p.id()
	return p.minLeftDFS(root, 0, p.n, r, f, &acc)
}

func (p *PST[T]) minLeftDFS(v, tl, tr, r int, f func(T) bool, acc *T) int {
	if r <= tl {
		return tl
	}
	if v == 0 {
		return tl
	}
	if tr <= r {
		nxt := p.op(p.st[v].val, *acc)
		if f(nxt) {
			*acc = nxt
			return tl
		}
		if tr-tl == 1 {
			return tr
		}
	}
	tm := (tl + tr) >> 1
	res := p.minLeftDFS(p.st[v].r, tm, tr, r, f, acc)
	if res > tm {
		return res
	}
	return p.minLeftDFS(p.st[v].l, tl, tm, r, f, acc)
}

// Memory is O(#updates * log2 n)
type PST1 struct {
	n  int        // positions 1..n
	st []PST1Node // node pool, st[0] = null node
}

type PST1Node struct {
	l   int // children indices in pool
	r   int
	sum int
}

func NewPST1(n int, reserveNodes int) *PST1 {
	p := &PST1{n: n}
	p.st = make([]PST1Node, 0, max(1, reserveNodes))
	p.st = append(p.st, PST1Node{}) // null node at index 0
	return p
}

func (p *PST1) clone(x PST1Node) int {
	p.st = append(p.st, x)
	return len(p.st) - 1
}

// Point add: return new root after adding delta at position pos
func (p *PST1) Add(root, pos, delta int) int {
	return p.add(root, 1, p.n, pos, delta)
}

func (p *PST1) add(v, tl, tr, pos, delta int) int {
	cur := p.st[v]
	if tl == tr {
		cur.sum += delta
		return p.clone(cur)
	}
	tm := (tl + tr) >> 1
	if pos <= tm {
		cur.l = p.add(cur.l, tl, tm, pos, delta)
	} else {
		cur.r = p.add(cur.r, tm+1, tr, pos, delta)
	}
	cur.sum = p.st[cur.l].sum + p.st[cur.r].sum
	return p.clone(cur)
}

// Range sum query on [l, r]
func (p *PST1) Query(root, l, r int) int {
	if l > r {
		return 0
	}
	return p.query(root, 1, p.n, l, r)
}

func (p *PST1) query(v, tl, tr, l, r int) int {
	if v == 0 || r < tl || l > tr {
		return 0
	}
	if l <= tl && tr <= r {
		return p.st[v].sum
	}
	tm := (tl + tr) >> 1
	return p.query(p.st[v].l, tl, tm, l, r) + p.query(p.st[v].r, tm+1, tr, l, r)
}

// Total sum in this version
func (p *PST1) Total(root int) int {
	return p.st[root].sum
}

// k-th order statistic: smallest position p where prefix sum >= k (1-indexed k)
// Requires: 1 <= k <= total(root) and all values non-negative
func (p *PST1) Kth(v, tl, tr, k int) int {
	if tl == tr {
		return tl
	}
	left := p.st[v].l
	leftSum := p.st[left].sum
	tm := (tl + tr) >> 1
	if k <= leftSum {
		return p.Kth(left, tl, tm, k)
	}
	return p.Kth(p.st[v].r, tm+1, tr, k-leftSum)
}
