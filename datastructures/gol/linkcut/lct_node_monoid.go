package linkcut

// Node abstraction from LCT
// Node will work with templated monoid
type LCTNodeMonoid[T any] struct {
	L   *LCTNodeMonoid[T]
	R   *LCTNodeMonoid[T]
	P   *LCTNodeMonoid[T]
	Idx int
	Siz int // size of aux splay subtree
	Rev bool

	VX T // value of this vertex
	X  T // aggregate in inorder: left -> self -> right
	RX T // aggregate in reversed inorder: right -> self -> left

	Op func(T, T) T
	Id func() T
}

func NewLCTNodeMonoid[T any](idx int, op func(T, T) T, id func() T) LCTNodeMonoid[T] {
	zero := id()
	return LCTNodeMonoid[T]{
		Idx: idx,
		Siz: 1,
		VX:  zero,
		X:   zero,
		RX:  zero,
		Op:  op,
		Id:  id,
	}
}

// update node from left and right children
func (n *LCTNodeMonoid[T]) Update() {
	n.Siz = 1
	n.X, n.RX = n.VX, n.VX
	if n.L != nil {
		n.Siz += n.L.Siz
		n.X = n.Op(n.L.X, n.X)
		n.RX = n.Op(n.RX, n.L.RX)
	}
	if n.R != nil {
		n.Siz += n.R.Siz
		n.X = n.Op(n.X, n.R.X)
		n.RX = n.Op(n.R.RX, n.RX)
	}
}

func (n *LCTNodeMonoid[T]) Reverse() {
	n.Rev = !n.Rev
	n.L, n.R = n.R, n.L
	n.X, n.RX = n.RX, n.X
}

func (n *LCTNodeMonoid[T]) Push() {
	if !n.Rev {
		return
	}
	if n.L != nil {
		n.L.Reverse()
	}
	if n.R != nil {
		n.R.Reverse()
	}
	n.Rev = false
}

func (n *LCTNodeMonoid[T]) Set(v T) { n.VX = v }

func (n *LCTNodeMonoid[T]) AddLight(*LCTNodeMonoid[T])   {}
func (n *LCTNodeMonoid[T]) EraseLight(*LCTNodeMonoid[T]) {}
func (n *LCTNodeMonoid[T]) ChangeLight(a, b *LCTNodeMonoid[T]) {
	if a != nil {
		n.EraseLight(a)
	}
	if b != nil {
		n.AddLight(b)
	}
}
