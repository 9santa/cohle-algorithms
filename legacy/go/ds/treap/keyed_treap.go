package treap

// Keyed treap, max-heap by priority
// Inorder sorted by key (less)
// Heap by pri (larger pri closer to root)
// Duplicate policy: keys <= node go LEFT, keys > node go RIGHT
// Ops Time complexity: O(log n)
type TreapKeyed[Key any, X any] struct {
	root *treapKeyedNode[Key, X]
	less func(Key, Key) bool
	op   func(X, X) X
	id   func() X
	rng  splitmix64
}

type treapKeyedNode[Key any, X any] struct {
	key  Key
	pri  uint32
	size int
	x    X
	prod X
	l    *treapKeyedNode[Key, X]
	r    *treapKeyedNode[Key, X]
}

func NewTreapKeyed[Key any, X any](
	less func(Key, Key) bool,
	op func(X, X) X,
	id func() X,
) *TreapKeyed[Key, X] {
	return &TreapKeyed[Key, X]{less: less, op: op, id: id, rng: newSplitMix64()}
}

func (t *TreapKeyed[Key, X]) Reset() {
	t.root = nil
}

func (t *TreapKeyed[Key, X]) Size() int { return t.size(t.root) }

func (t *TreapKeyed[Key, X]) ProdAll() X { return t.prod(t.root) }

// O(log n)
// Range aggregate over keys in [l, r)
func (t *TreapKeyed[Key, X]) ProdRange(L, R Key) X {
	a, b := t.splitLt(t.root, L) // a: < L, b: >= L
	b, c := t.splitLt(b, R)      // b: [L, R), c: >= R
	ans := t.prod(b)
	t.root = t.merge3(a, b, c)
	return ans
}

// Range aggregate over keys in [l, r]
func (t *TreapKeyed[Key, X]) ProdRangeInclusive(L, R Key) X {
	if t.less(R, L) {
		return t.id()
	}
	a, b := t.splitLt(t.root, L)
	b, c := t.splitLeq(b, R)
	ans := t.prod(b)
	t.root = t.merge3(a, b, c)
	return ans
}

// Prefix prod: nodes with key <= k
func (t *TreapKeyed[Key, X]) ProdLeq(k Key) X {
	cur := t.root
	res := t.id()
	for cur != nil {
		if t.leq(cur.key, k) { // cur->key <= k
			res = t.op(res, t.prod(cur.l))
			res = t.op(res, cur.x)
			cur = cur.r
		} else {
			cur = cur.l
		}
	}
	return res
}

// max_right / min_left work by inorder position rather than key.
func (t *TreapKeyed[Key, X]) MaxRight(l int, pred func(X) bool) int {
	if !(0 <= l && l <= t.Size()) {
		panic("MaxRight: invalid index")
	}
	if !pred(t.id()) {
		panic("MaxRight: pred(id) must be true")
	}
	a, b := t.splitByOrder(t.root, l)
	acc := t.id()
	take := t.takePrefix(b, &acc, pred)
	t.root = t.merge(a, b)
	return l + take
}

func (t *TreapKeyed[Key, X]) MinLeft(r int, pred func(X) bool) int {
	if !(0 <= r && r <= t.Size()) {
		panic("MinLeft: invalid index")
	}
	if !pred(t.id()) {
		panic("MinLeft: pred(id) must be true")
	}
	a, b := t.splitByOrder(t.root, r)
	acc := t.id()
	take := t.takeSuffix(a, &acc, pred)
	t.root = t.merge(a, b)
	return r - take
}

// multiset insert: rotation-based
func (t *TreapKeyed[Key, X]) Insert(key Key, v X) {
	t.root = t.insert(t.root, key, v, t.rng.Next())
}

func (t *TreapKeyed[Key, X]) InsertWithPriority(key Key, v X, p uint32) {
	t.root = t.insert(t.root, key, v, p)
}

// Add merges into one existing equal-key node when present, otherwise inserts a new node.
func (t *TreapKeyed[Key, X]) Add(key Key, delta X) {
	t.root = t.add(t.root, key, delta)
}

// Erase one occurrence by key. Returns whether erased
func (t *TreapKeyed[Key, X]) EraseOne(key Key) bool {
	erased := false
	t.root = t.eraseOne(t.root, key, &erased)
	return erased
}

// Count occurrences by key
func (t *TreapKeyed[Key, X]) Count(key Key) int {
	a, bc := t.splitLt(t.root, key) // < key, >= key
	b, c := t.splitLeq(bc, key)     // <= key, > key
	ans := t.size(b)
	t.root = t.merge(a, t.merge(b, c))
	return ans
}

func (t *TreapKeyed[Key, X]) CountLeq(key Key) int {
	cur := t.root
	res := 0
	for cur != nil {
		if t.less(key, cur.key) {
			cur = cur.l
		} else {
			res += t.size(cur.l) + 1
			cur = cur.r
		}
	}
	return res
}

// lower_bound: first key >= k
func (t *TreapKeyed[Key, X]) LowerBound(key Key) *treapKeyedNode[Key, X] {
	cur := t.root
	var ans *treapKeyedNode[Key, X]
	for cur != nil {
		if !t.less(cur.key, key) {
			ans = cur
			cur = cur.l
		} else {
			cur = cur.r
		}
	}
	return ans
}

// #keys < k
func (t *TreapKeyed[Key, X]) OrderOfKey(key Key) int {
	cur := t.root
	res := 0
	for cur != nil {
		if t.less(cur.key, key) { // cur->key < key
			res += t.size(cur.l) + 1
			cur = cur.r
		} else {
			cur = cur.l
		}
	}
	return res
}

func (t *TreapKeyed[Key, X]) size(n *treapKeyedNode[Key, X]) int {
	if n == nil {
		return 0
	}
	return n.size
}

func (t *TreapKeyed[Key, X]) prod(n *treapKeyedNode[Key, X]) X {
	if n == nil {
		return t.id()
	}
	return n.prod
}

func (t *TreapKeyed[Key, X]) pull(n *treapKeyedNode[Key, X]) {
	if n == nil {
		return
	}
	n.size = 1 + t.size(n.l) + t.size(n.r)
	n.prod = t.op(t.op(t.prod(n.l), n.x), t.prod(n.r))
}

// Define <= via !(b < a)
func (t *TreapKeyed[Key, X]) leq(a, b Key) bool { return !t.less(b, a) }

// Define ==
func (t *TreapKeyed[Key, X]) equal(a, b Key) bool { return !t.less(a, b) && !t.less(b, a) }

// Rotate right: bring left child up
// Rotations preserve in-order traversal sequence
func (t *TreapKeyed[Key, X]) rotR(n *treapKeyedNode[Key, X]) *treapKeyedNode[Key, X] {
	p := n.l
	n.l = p.r
	p.r = n
	t.pull(n)
	t.pull(p)
	return p
}

// Rotate left: bring right child up
func (t *TreapKeyed[Key, X]) rotL(n *treapKeyedNode[Key, X]) *treapKeyedNode[Key, X] {
	p := n.r
	n.r = p.l
	p.l = n
	t.pull(n)
	t.pull(p)
	return p
}

// Rotation-based insert:
// 1) BST insert by key
// 2) rotate if child priority exceeds parent (for max-heap)
func (t *TreapKeyed[Key, X]) insert(n *treapKeyedNode[Key, X], key Key, v X, p uint32) *treapKeyedNode[Key, X] {
	if n == nil {
		return &treapKeyedNode[Key, X]{key: key, pri: p, size: 1, x: v, prod: v}
	}
	if t.leq(key, n.key) {
		n.l = t.insert(n.l, key, v, p)
		if n.l.pri > n.pri {
			n = t.rotR(n)
		}
	} else {
		n.r = t.insert(n.r, key, v, p)
		if n.r.pri > n.pri {
			n = t.rotL(n)
		}
	}
	t.pull(n)
	return n
}

func (t *TreapKeyed[Key, X]) add(n *treapKeyedNode[Key, X], key Key, delta X) *treapKeyedNode[Key, X] {
	if n == nil {
		return &treapKeyedNode[Key, X]{key: key, pri: t.rng.Next(), size: 1, x: delta, prod: delta}
	}
	if t.equal(key, n.key) {
		n.x = t.op(n.x, delta)
		t.pull(n)
		return n
	}
	if t.leq(key, n.key) {
		n.l = t.add(n.l, key, delta)
		if n.l.pri > n.pri {
			n = t.rotR(n)
		}
	} else {
		n.r = t.add(n.r, key, delta)
		if n.r.pri > n.pri {
			n = t.rotL(n)
		}
	}
	t.pull(n)
	return n
}

// Rotation-based erase-one:
// Find a node with key=k; then rotate it down until it becomes a leaf
// then remove it
func (t *TreapKeyed[Key, X]) eraseOne(n *treapKeyedNode[Key, X], key Key, erased *bool) *treapKeyedNode[Key, X] {
	if n == nil {
		return nil
	}
	if t.equal(n.key, key) {
		*erased = true
		if n.l == nil && n.r == nil {
			return nil
		}
		if n.l == nil {
			return n.r
		}
		if n.r == nil {
			return n.l
		}
		// both children exist: rotate the higher-priority child up
		if n.l.pri > n.r.pri {
			n = t.rotR(n)
			n.r = t.eraseOne(n.r, key, erased)
		} else {
			n = t.rotL(n)
			n.l = t.eraseOne(n.l, key, erased)
		}
		t.pull(n)
		return n
	}
	// standard BST descent
	if t.leq(key, n.key) {
		n.l = t.eraseOne(n.l, key, erased)
	} else {
		n.r = t.eraseOne(n.r, key, erased)
	}
	t.pull(n)
	return n
}

// split by key; split_lt = split_less_than: (< key) and (>= key)
func (t *TreapKeyed[Key, X]) splitLt(n *treapKeyedNode[Key, X], key Key) (*treapKeyedNode[Key, X], *treapKeyedNode[Key, X]) {
	if n == nil {
		return nil, nil
	}
	if t.less(n.key, key) { // node->key < key -> split the right subtree
		a, b := t.splitLt(n.r, key) // a: < key, b: >= key
		n.r = a                     // cut the tree: now whole n < key
		t.pull(n)
		return n, b
	}
	a, b := t.splitLt(n.l, key)
	n.l = b
	t.pull(n)
	return a, n
}

// split by key; split_leq = split_less_or_equal: (<= key) and (> key)
func (t *TreapKeyed[Key, X]) splitLeq(n *treapKeyedNode[Key, X], key Key) (*treapKeyedNode[Key, X], *treapKeyedNode[Key, X]) {
	if n == nil {
		return nil, nil
	}
	if t.leq(n.key, key) {
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

// split by order: [0, k) and [k, n)
func (t *TreapKeyed[Key, X]) splitByOrder(n *treapKeyedNode[Key, X], k int) (*treapKeyedNode[Key, X], *treapKeyedNode[Key, X]) {
	if n == nil {
		return nil, nil
	}
	if t.size(n.l) >= k {
		a, b := t.splitByOrder(n.l, k)
		n.l = b
		t.pull(n)
		return a, n
	}
	a, b := t.splitByOrder(n.r, k-t.size(n.l)-1)
	n.r = a
	t.pull(n)
	return n, b
}

// merge assumes all keys in a <= all keys in b
func (t *TreapKeyed[Key, X]) merge(a, b *treapKeyedNode[Key, X]) *treapKeyedNode[Key, X] {
	if a == nil {
		return b
	}
	if b == nil {
		return a
	}
	if a.pri > b.pri {
		a.r = t.merge(a.r, b)
		t.pull(a)
		return a
	}
	b.l = t.merge(a, b.l)
	t.pull(b)
	return b
}

func (t *TreapKeyed[Key, X]) merge3(a, b, c *treapKeyedNode[Key, X]) *treapKeyedNode[Key, X] {
	return t.merge(t.merge(a, b), c)
}

func (t *TreapKeyed[Key, X]) takePrefix(n *treapKeyedNode[Key, X], acc *X, pred func(X) bool) int {
	if n == nil {
		return 0
	}
	// try take entire left subtree
	if n.l != nil {
		cand := t.op(*acc, t.prod(n.l))
		if !pred(cand) {
			return t.takePrefix(n.l, acc, pred)
		}
		*acc = cand
	}
	// try take current node
	cand := t.op(*acc, n.x)
	if !pred(cand) {
		return t.size(n.l)
	}
	*acc = cand
	// then take from right
	res := t.size(n.l) + 1
	if n.r != nil {
		res += t.takePrefix(n.r, acc, pred)
	}
	return res
}

func (t *TreapKeyed[Key, X]) takeSuffix(n *treapKeyedNode[Key, X], acc *X, pred func(X) bool) int {
	if n == nil {
		return 0
	}
	// try take entire right subtree
	if n.r != nil {
		cand := t.op(t.prod(n.r), *acc)
		if !pred(cand) {
			return t.takeSuffix(n.r, acc, pred)
		}
		*acc = cand
	}
	// try take current node
	cand := t.op(n.x, *acc)
	if !pred(cand) {
		return t.size(n.r)
	}
	*acc = cand
	// then take from left
	res := t.size(n.r) + 1
	if n.l != nil {
		res += t.takeSuffix(n.l, acc, pred)
	}
	return res
}
