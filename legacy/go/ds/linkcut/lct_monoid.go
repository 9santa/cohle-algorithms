package linkcut

type LCTMonoid[T any] struct {
	n  int
	t  []LCTNodeMonoid[T]
	op func(T, T) T
	id func() T
}

func NewLCTMonoid[T any](n int, op func(T, T) T, id func() T) *LCTMonoid[T] {
	l := &LCTMonoid[T]{op: op, id: id}
	l.Init(n)
	return l
}

func (l *LCTMonoid[T]) Init(n int) {
	l.n = n
	l.t = make([]LCTNodeMonoid[T], n)
	for i := 0; i < n; i++ {
		l.t[i] = NewLCTNodeMonoid(i, l.op, l.id)
	}
}

func (l *LCTMonoid[T]) Node(v int) *LCTNodeMonoid[T] {
	return &l.t[v]
}

func (l *LCTMonoid[T]) isRoot(x *LCTNodeMonoid[T]) bool {
	return x.P == nil || (x.P.L != x && x.P.R != x)
}

func (l *LCTMonoid[T]) pull(x *LCTNodeMonoid[T]) {
	if x != nil {
		x.Update()
	}
}

func (l *LCTMonoid[T]) push(x *LCTNodeMonoid[T]) {
	if x != nil {
		x.Push()
	}
}

func (l *LCTMonoid[T]) pushTo(x *LCTNodeMonoid[T]) {
	stk := make([]*LCTNodeMonoid[T], 0)
	for y := x; ; y = y.P {
		stk = append(stk, y)
		if l.isRoot(y) {
			break
		}
	}
	for i := len(stk) - 1; i >= 0; i-- {
		l.push(stk[i])
	}
}

func (l *LCTMonoid[T]) rotate(x *LCTNodeMonoid[T]) {
	p := x.P
	g := p.P
	dirx := p.R == x // is x right child of p?
	var b *LCTNodeMonoid[T]
	if dirx {
		b = x.L
	} else {
		b = x.R
	}

	if !l.isRoot(p) {
		if g.L == p {
			g.L = x
		} else if g.R == p {
			g.R = x
		}
	}
	x.P = g

	if dirx {
		x.L = p
		p.P = x
		p.R = b
		if b != nil {
			b.P = p
		}
	} else {
		x.R = p
		p.P = x
		p.L = b
		if b != nil {
			b.P = p
		}
	}

	l.pull(p)
	l.pull(x)
}

func (l *LCTMonoid[T]) splay(x *LCTNodeMonoid[T]) {
	l.pushTo(x)
	for !l.isRoot(x) {
		p := x.P
		g := p.P
		if !l.isRoot(p) {
			zigzig := (p.L == x) == (g.L == p)
			if zigzig {
				l.rotate(p)
			} else {
				l.rotate(x)
			}
		}
		l.rotate(x)
	}
}

// makes the path (represented tree root ... x) preferred
// returns last accessed node (useful for lca)
func (l *LCTMonoid[T]) Expose(x *LCTNodeMonoid[T]) *LCTNodeMonoid[T] {
	var last *LCTNodeMonoid[T]
	for y := x; y != nil; y = y.P {
		l.splay(y)

		oldR := y.R

		// old preferred child becomes virtual
		if oldR != nil {
			y.AddLight(oldR)
		}

		// new preferred child stops being virtual
		if last != nil {
			y.EraseLight(last)
		}

		y.R = last
		if last != nil {
			last.P = y
		}

		l.pull(y)
		last = y
	}
	l.splay(x)
	return last
}

func (l *LCTMonoid[T]) Access(x *LCTNodeMonoid[T]) *LCTNodeMonoid[T] { return l.Expose(x) }

func (l *LCTMonoid[T]) Evert(x *LCTNodeMonoid[T]) {
	l.Expose(x)
	x.Reverse()
}

func (l *LCTMonoid[T]) Makeroot(x *LCTNodeMonoid[T]) { l.Evert(x) }

func (l *LCTMonoid[T]) Findroot(x *LCTNodeMonoid[T]) *LCTNodeMonoid[T] {
	l.Expose(x)
	for x.L != nil {
		x.Push()
		x = x.L
	}
	l.splay(x)
	return x
}

func (l *LCTMonoid[T]) Connected(a, b *LCTNodeMonoid[T]) bool {
	if a == b {
		return true
	}
	return l.Findroot(a) == l.Findroot(b)
}

// after split(a, b), the whole path a->b is exactly the aux tree rooted at b
func (l *LCTMonoid[T]) Split(a, b *LCTNodeMonoid[T]) bool {
	if !l.Connected(a, b) {
		return false
	}
	l.Makeroot(a)
	l.Expose(b)
	return true
}

func (l *LCTMonoid[T]) Link(a, b *LCTNodeMonoid[T]) bool {
	l.Makeroot(a)
	if l.Findroot(b) == a {
		return false // already connected
	}

	// a becomes a virtual child of b
	l.Expose(b)
	a.P = b
	b.AddLight(a)
	l.pull(b)
	return true
}

func (l *LCTMonoid[T]) Cut(a, b *LCTNodeMonoid[T]) bool {
	l.Makeroot(a)
	l.Expose(b)

	// if edge a-b exists directly, then a is left child of b
	// and a has no right child
	if b.L != a || a.R != nil {
		return false
	}

	b.L = nil
	a.P = nil
	l.pull(b)
	return true
}

func (l *LCTMonoid[T]) LCA(a, b *LCTNodeMonoid[T]) *LCTNodeMonoid[T] {
	if !l.Connected(a, b) {
		return nil
	}
	l.Expose(a)
	return l.Expose(b)
}

func (l *LCTMonoid[T]) Set(x *LCTNodeMonoid[T], v T) {
	l.Expose(x)
	x.Set(v)
	l.pull(x)
}

func (l *LCTMonoid[T]) GetVertex(x *LCTNodeMonoid[T]) T {
	l.Expose(x)
	return x.VX
}

// aggregate on path a ... b
func (l *LCTMonoid[T]) ProdPath(a, b *LCTNodeMonoid[T]) T {
	l.Split(a, b)
	return b.X
}

// aggregate on path b .. a
func (l *LCTMonoid[T]) ProdPathRev(a, b *LCTNodeMonoid[T]) T {
	l.Split(a, b)
	return b.RX
}
