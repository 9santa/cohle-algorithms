package treap

// Standard treap for multiset of points (X, Y): BST by X, max-heap by Y.
// Duplicate policy: left <= X0, right > X0, (duplicates go LEFT).
type TreapXY[T any] struct {
	root *treapXYNode[T]
	less func(T, T) bool
	rng  splitmix64
}

type treapXYNode[T any] struct {
	x    T
	y    uint32
	size int
	l    *treapXYNode[T]
	r    *treapXYNode[T]
}

func NewTreapXY[T any](less func(T, T) bool) *TreapXY[T] {
	return &TreapXY[T]{less: less, rng: newSplitMix64()}
}

func (t *TreapXY[T]) Reset() {
	t.root = nil
}

func (t *TreapXY[T]) Size() int { return t.size(t.root) }

func (t *TreapXY[T]) size(n *treapXYNode[T]) int {
	if n == nil {
		return 0
	}
	return n.size
}

func (t *TreapXY[T]) pull(n *treapXYNode[T]) {
	if n == nil {
		return
	}
	n.size = 1 + t.size(n.l) + t.size(n.r)
}

// a <= b with comparator
func (t *TreapXY[T]) leq(a, b T) bool {
	return !t.less(b, a) // !(b < a)
}

// O(log n)
// split by key (dups go left): [<= key] and [> key]
func (t *TreapXY[T]) splitLeq(n *treapXYNode[T], key T) (*treapXYNode[T], *treapXYNode[T]) {
	if n == nil {
		return nil, nil
	}
	if t.leq(n.x, key) {
		// n->l, n are <= key; n->r MIGHT contain elements both <= key and > key
		a, b := t.splitLeq(n.r, key)
		n.r = a
		t.pull(n)
		return n, b
	}
	a, b := t.splitLeq(n.l, key)
	n.l = b
	t.pull(n)
	return a, n
}

// O(log n)
// split by key (< key): [< key] and [>= key]
func (t *TreapXY[T]) splitLt(n *treapXYNode[T], key T) (*treapXYNode[T], *treapXYNode[T]) {
	if n == nil {
		return nil, nil
	}
	if t.less(n.x, key) {
		a, b := t.splitLt(n.r, key)
		n.r = a
		t.pull(n)
		return n, b
	}
	a, b := t.splitLt(n.l, key)
	n.l = b
	t.pull(n)
	return a, n
}

// O(log n)
// merge: assumes all keys in a <= all keys in b (under chosen policy)
func (t *TreapXY[T]) merge(a, b *treapXYNode[T]) *treapXYNode[T] {
	if a == nil {
		return b
	}
	if b == nil {
		return a
	}
	// max-heap by y: root is bigger y
	if a.y >= b.y {
		a.r = t.merge(a.r, b)
		t.pull(a)
		return a
	}
	b.l = t.merge(a, b.l)
	t.pull(b)
	return b
}

// O(log n)
// insert (X with optional Y). Random priority if Y isn't provided
func (t *TreapXY[T]) InsertWithPriority(x T, y uint32) {
	nd := &treapXYNode[T]{x: x, y: y, size: 1}
	a, b := t.splitLeq(t.root, x) // dups go left
	t.root = t.merge(t.merge(a, nd), b)
}

func (t *TreapXY[T]) Insert(x T) { t.InsertWithPriority(x, t.rng.Next()) }

// O(log n)
// erase one occurence of key (multiset erase-one)
func (t *TreapXY[T]) EraseOne(key T) bool {
	a, bc := t.splitLt(t.root, key) // a < key, bc >= key
	b, c := t.splitLeq(bc, key)     // b == key, c > key
	if b == nil {
		t.root = t.merge(a, t.merge(b, c))
		return false
	}
	del := b
	b = t.merge(b.l, b.r)
	del.l, del.r = nil, nil
	t.root = t.merge(a, t.merge(b, c))
	return true
}

// O(log n)
// lower_bound: first element with x >= key
// Returns pointer to node (do not store if treap will change)
func (t *TreapXY[T]) LowerBound(key T) *treapXYNode[T] {
	cur := t.root
	var ans *treapXYNode[T]
	for cur != nil {
		if !t.less(cur.x, key) { // cur.x >= key
			ans = cur
			cur = cur.l
		} else {
			cur = cur.r
		}
	}
	return ans
}

// count occurences (multiset)
func (t *TreapXY[T]) Count(key T) int {
	a, bc := t.splitLt(t.root, key)
	b, c := t.splitLeq(bc, key)
	res := t.size(b)
	t.root = t.merge(a, t.merge(b, c))
	return res
}
