package treap

// Interval tree with treap balancing
// BST key: L (segment start)
// Each node stores interval [L, R) and augmented mxR = max R in subtree
type IntervalTreap[Coord any] struct {
	root *intervalTreapNode[Coord]
	less func(Coord, Coord) bool
	rng  splitmix64
}

type intervalTreapNode[Coord any] struct {
	L    Coord
	R    Coord
	mxR  Coord
	pri  uint32
	size int
	l    *intervalTreapNode[Coord]
	r    *intervalTreapNode[Coord]
}

func NewIntervalTreap[Coord any](less func(Coord, Coord) bool) *IntervalTreap[Coord] {
	return &IntervalTreap[Coord]{less: less, rng: newSplitMix64()}
}

func (t *IntervalTreap[Coord]) Reset() {
	t.root = nil
}

// Insert assumes unique L
func (t *IntervalTreap[Coord]) Insert(L, R Coord) {
	if t.less(R, L) {
		panic("Insert: require L <= R")
	}
	t.root = t.ins(t.root, t.newNode(L, R))
}

func (t *IntervalTreap[Coord]) EraseByL(L Coord) bool {
	erased := false
	t.root = t.era(t.root, L, &erased)
	return erased
}

// Returns pointer to an interval containing x, or nil
func (t *IntervalTreap[Coord]) FindContaining(x Coord) *intervalTreapNode[Coord] {
	cur := t.root
	for cur != nil {
		// if left subtree has some interval with R > x, the answer might be there
		if cur.l != nil && t.less(x, cur.l.mxR) {
			cur = cur.l
			continue
		}
		// check current interval: L <= x < R
		if !t.less(x, cur.L) && t.less(x, cur.R) {
			return cur
		}
		cur = cur.r
	}
	return nil
}

func (t *IntervalTreap[Coord]) ContainsPoint(x Coord) bool {
	return t.FindContaining(x) != nil
}

// Any overlap with query [L, R): overlap if node.L < R && L < node.R
func (t *IntervalTreap[Coord]) AnyOverlap(L, R Coord) bool {
	return t.anyOverlapRec(t.root, L, R)
}

func (t *IntervalTreap[Coord]) newNode(L, R Coord) *intervalTreapNode[Coord] {
	return &intervalTreapNode[Coord]{L: L, R: R, mxR: R, pri: t.rng.Next(), size: 1}
}

func (t *IntervalTreap[Coord]) size(n *intervalTreapNode[Coord]) int {
	if n == nil {
		return 0
	}
	return n.size
}

func (t *IntervalTreap[Coord]) equal(a, b Coord) bool {
	return !t.less(a, b) && !t.less(b, a)
}

func (t *IntervalTreap[Coord]) pull(n *intervalTreapNode[Coord]) {
	if n == nil {
		return
	}
	n.size = 1 + t.size(n.l) + t.size(n.r)
	n.mxR = n.R
	if n.l != nil && t.less(n.mxR, n.l.mxR) {
		n.mxR = n.l.mxR
	}
	if n.r != nil && t.less(n.mxR, n.r.mxR) {
		n.mxR = n.r.mxR
	}
}

// Rotate right: bring left child up
// Rotations preserve in-order traversal sequence
func (t *IntervalTreap[Coord]) rotR(n *intervalTreapNode[Coord]) *intervalTreapNode[Coord] {
	p := n.l
	n.l = p.r
	p.r = n
	t.pull(n)
	t.pull(p)
	return p
}

// Rotate left: bring right child up
func (t *IntervalTreap[Coord]) rotL(n *intervalTreapNode[Coord]) *intervalTreapNode[Coord] {
	p := n.r
	n.r = p.l
	p.l = n
	t.pull(n)
	t.pull(p)
	return p
}

// Rotation-based insert:
// 1) BST by L
// 2) rotate if child priority exceeds parent (for max-heap)
func (t *IntervalTreap[Coord]) ins(cur, nd *intervalTreapNode[Coord]) *intervalTreapNode[Coord] {
	if cur == nil {
		t.pull(nd)
		return nd
	}
	if t.less(nd.L, cur.L) {
		cur.l = t.ins(cur.l, nd)
		if cur.l.pri > cur.pri {
			cur = t.rotR(cur)
		}
	} else {
		cur.r = t.ins(cur.r, nd)
		if cur.r.pri > cur.pri {
			cur = t.rotL(cur)
		}
	}
	t.pull(cur)
	return cur
}

// Rotation-based erase-one:
// Find a node with key=L; then rotate it down until it becomes a leaf
// then remove it
func (t *IntervalTreap[Coord]) era(cur *intervalTreapNode[Coord], L Coord, erased *bool) *intervalTreapNode[Coord] {
	if cur == nil {
		return nil
	}
	if t.equal(cur.L, L) {
		*erased = true
		if cur.l == nil && cur.r == nil {
			return nil
		}
		if cur.l == nil {
			return cur.r
		}
		if cur.r == nil {
			return cur.l
		}
		// both children exist: rotate the higher-priority child up
		if cur.l.pri > cur.r.pri {
			cur = t.rotR(cur)
			cur.r = t.era(cur.r, L, erased)
		} else {
			cur = t.rotL(cur)
			cur.l = t.era(cur.l, L, erased)
		}
		t.pull(cur)
		return cur
	}
	if t.less(L, cur.L) {
		cur.l = t.era(cur.l, L, erased)
	} else {
		cur.r = t.era(cur.r, L, erased)
	}
	t.pull(cur)
	return cur
}

func (t *IntervalTreap[Coord]) overlapOne(cur *intervalTreapNode[Coord], L, R Coord) bool {
	return t.less(cur.L, R) && t.less(L, cur.R)
}

func (t *IntervalTreap[Coord]) anyOverlapRec(cur *intervalTreapNode[Coord], L, R Coord) bool {
	if cur == nil {
		return false
	}
	// if left subtree exists and its mxR > L, it might contain an interval overlapping [L, R)
	if cur.l != nil && t.less(L, cur.l.mxR) {
		if t.anyOverlapRec(cur.l, L, R) {
			return true
		}
	}
	if t.overlapOne(cur, L, R) {
		return true
	}
	// prune right subtree: if cur.L >= R, everything on right starts even later -> no overlap
	if !t.less(cur.L, R) {
		return false
	}
	return t.anyOverlapRec(cur.r, L, R)
}
