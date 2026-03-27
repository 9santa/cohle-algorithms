package treap

const beatsInf int64 = 4_000_000_000_000_000_000

type BeatsLazy struct {
	rev  bool  // toggles order (swap children)
	val  int64 // val is the increment amount
	inc  bool  // bool for whether this tag increments (inc = false means set)
	kval int
	kinc bool
	mn   int64 // pending lower bound: after applying the tag, values should be <= mn (chmin)
	mx   int64 // pending upper bound: after applying the tag, values should be >= mx (chmax)
	add  int64 // pending addition to segment
}

func BeatsLazyIdentity() BeatsLazy {
	return BeatsLazy{
		inc:  true,
		kinc: true,
		mn:   beatsInf,
		mx:   -beatsInf,
	}
}

// composes two lazy tags: apply 'oth' after this
func (z *BeatsLazy) Compose(oth BeatsLazy) {
	z.rev = z.rev != oth.rev

	if !oth.inc {
		z.val = 0
		z.inc = false
	}
	z.val += oth.val

	if !oth.kinc {
		z.kval = 0
		z.kinc = false
	}
	z.kval += oth.kval

	// 'beats' clamp with pending add:
	// we store mn/mx with bounds in the same coord system as values AFTER add
	// i.e. chmin/chmax respect pending add, as if it is applied
	if oth.mn-z.add <= z.mx {
		z.mn = oth.mn - z.add
		z.mx = oth.mn - z.add
	} else if oth.mx-z.add >= z.mn {
		z.mn = oth.mx - z.add
		z.mx = oth.mx - z.add
	} else {
		if z.mn > oth.mn-z.add {
			z.mn = oth.mn - z.add
		}
		if z.mx < oth.mx-z.add {
			z.mx = oth.mx - z.add
		}
	}
	z.add += oth.add
}

func BeatsChminTag(x int64) BeatsLazy {
	t := BeatsLazyIdentity()
	t.mn = x
	return t
}

func BeatsChmaxTag(x int64) BeatsLazy {
	t := BeatsLazyIdentity()
	t.mx = x
	return t
}

func BeatsAddTag(x int64) BeatsLazy {
	t := BeatsLazyIdentity()
	t.add = x
	return t
}

func BeatsRevTag() BeatsLazy {
	t := BeatsLazyIdentity()
	t.rev = true
	return t
}

type BeatsValue struct {
	ksum  int
	sum   int64
	mx    int64
	mxcnt int64
	mx2   int64
	mn    int64
	mncnt int64
	mn2   int64
}

func MakeBeatsValue(x int64, length int64) BeatsValue {
	return BeatsValue{
		sum:   x * length,
		mx:    x,
		mxcnt: length,
		mx2:   -beatsInf,
		mn:    x,
		mncnt: length,
		mn2:   beatsInf,
	}
}

func MakeBeatsValue1(x int64) BeatsValue {
	return MakeBeatsValue(x, 1)
}

func BeatsValueIdentity() BeatsValue {
	return BeatsValue{
		mx:  -beatsInf,
		mx2: -beatsInf,
		mn:  beatsInf,
		mn2: beatsInf,
	}
}

// Tag does nothing on this segment => stop recursion
func (v BeatsValue) CanBreak(lazy BeatsLazy) bool {
	return lazy.mn >= v.mx && lazy.mx <= v.mn && lazy.add == 0
}

// Safe to apply tag without descending
func (v BeatsValue) CanTag(lazy BeatsLazy) bool {
	return v.mx2 < lazy.mn && v.mn2 > lazy.mx
}

// Apply lazy to this segment, O(1)
func (v *BeatsValue) Apply(lazy BeatsLazy, sz int) {
	if !lazy.kinc {
		v.ksum = 0
	}
	v.ksum += int(lazy.val) * sz

	// apply chmin/chmax
	if v.mn == v.mx {
		if v.mn > lazy.mn {
			v.mn = lazy.mn
			v.mx = lazy.mn
		}
		if v.mn < lazy.mx {
			v.mn = lazy.mx
			v.mx = lazy.mx
		}
		v.sum = v.mn * v.mncnt
	} else if v.mn == v.mx2 {
		if lazy.mx > v.mn {
			v.mn = lazy.mx
			v.mx2 = lazy.mx
		}
		if lazy.mn < v.mx {
			v.mx = lazy.mn
			v.mn2 = lazy.mn
		}
		v.sum = v.mn*v.mncnt + v.mx*v.mxcnt
	} else {
		if lazy.mn < v.mx {
			v.sum -= (v.mx - lazy.mn) * v.mxcnt
			v.mx = lazy.mn
		}
		if lazy.mx > v.mn {
			v.sum += (lazy.mx - v.mn) * v.mncnt
			v.mn = lazy.mx
		}
	}

	// apply add
	v.sum += lazy.add * int64(sz)
	v.mx += lazy.add
	v.mx2 += lazy.add
	v.mn += lazy.add
	v.mn2 += lazy.add
}

func (v BeatsValue) Plus(oth BeatsValue) BeatsValue {
	res := BeatsValue{}
	res.ksum = v.ksum + oth.ksum
	res.sum = v.sum + oth.sum

	// max merge
	if v.mx == oth.mx {
		res.mx = v.mx
		res.mxcnt = v.mxcnt + oth.mxcnt
		res.mx2 = maxInt64(v.mx2, oth.mx2)
	} else if v.mx > oth.mx {
		res.mx = v.mx
		res.mxcnt = v.mxcnt
		res.mx2 = maxInt64(v.mx2, oth.mx)
	} else {
		res.mx = oth.mx
		res.mxcnt = oth.mxcnt
		res.mx2 = maxInt64(v.mx, oth.mx2)
	}

	// min merge
	if v.mn == oth.mn {
		res.mn = v.mn
		res.mncnt = v.mncnt + oth.mncnt
		res.mn2 = minInt64(v.mn2, oth.mn2)
	} else if v.mn < oth.mn {
		res.mn = v.mn
		res.mncnt = v.mncnt
		res.mn2 = minInt64(v.mn2, oth.mn)
	} else {
		res.mn = oth.mn
		res.mncnt = oth.mncnt
		res.mn2 = minInt64(v.mn, oth.mn2)
	}

	return res
}

// Implicit Treap Beats: range ops by index [l, r)
type LazyBeatsTreap struct {
	root *lazyBeatsTreapNode
	rng  splitmix64
}

type lazyBeatsTreapNode struct {
	val BeatsValue // single element value
	agg BeatsValue // subtree aggregate
	lz  BeatsLazy  // pending lazy to push to children (val/agg are updated when lazy is applied to the node)

	siz int
	pri uint32
	l   *lazyBeatsTreapNode
	r   *lazyBeatsTreapNode
}

func NewLazyBeatsTreap() *LazyBeatsTreap {
	return &LazyBeatsTreap{rng: newSplitMix64()}
}

func (t *LazyBeatsTreap) Size() int { return t.siz(t.root) }

func (t *LazyBeatsTreap) siz(n *lazyBeatsTreapNode) int {
	if n == nil {
		return 0
	}
	return n.siz
}

func (t *LazyBeatsTreap) agg(n *lazyBeatsTreapNode) BeatsValue {
	if n == nil {
		return BeatsValueIdentity()
	}
	return n.agg
}

func (t *LazyBeatsTreap) isLazyIdentity(z BeatsLazy) bool {
	base := BeatsLazyIdentity()
	return !z.rev && z.add == 0 && z.mn == base.mn && z.mx == base.mx &&
		z.val == 0 && z.kval == 0 && z.inc && z.kinc
}

func (t *LazyBeatsTreap) Build(a []int64) {
	t.Clear()
	if len(a) == 0 {
		return
	}
	st := make([]*lazyBeatsTreapNode, 0, len(a))
	for _, v := range a {
		cur := &lazyBeatsTreapNode{val: MakeBeatsValue1(v), agg: MakeBeatsValue1(v), lz: BeatsLazyIdentity(), siz: 1, pri: t.rng.Next()}
		var last *lazyBeatsTreapNode
		for len(st) > 0 && st[len(st)-1].pri < cur.pri {
			last = st[len(st)-1]
			st = st[:len(st)-1]
			t.pull(last)
		}
		cur.l = last
		if len(st) > 0 {
			st[len(st)-1].r = cur
		}
		st = append(st, cur)
	}
	t.root = st[0] // root is bottom of stack

	// pull all nodes in reverse topological order via iterative postorder
	order := make([]*lazyBeatsTreapNode, 0, len(a))
	stack := []*lazyBeatsTreapNode{t.root}
	for len(stack) > 0 {
		n := stack[len(stack)-1]
		stack = stack[:len(stack)-1]
		order = append(order, n)
		if n.l != nil {
			stack = append(stack, n.l)
		}
		if n.r != nil {
			stack = append(stack, n.r)
		}
	}
	for i := len(order) - 1; i >= 0; i-- {
		t.pull(order[i])
	}
}

// apply tag to a node in O(1)
// - update val and agg immediately (so queries can read values without forcing a full push)
// - accumulate tag into lz for future propagation
func (t *LazyBeatsTreap) applyAt(n *lazyBeatsTreapNode, z BeatsLazy) {
	if n == nil || t.isLazyIdentity(z) {
		return
	}
	n.val.Apply(z, 1)
	n.agg.Apply(z, t.siz(n))
	n.lz.Compose(z)
}

// Push lazy tag down 1 level:
// 1) if rev: swap children
// 2) propagate tag to children (composition)
func (t *LazyBeatsTreap) push(n *lazyBeatsTreapNode) {
	if n == nil || t.isLazyIdentity(n.lz) {
		return
	}
	// if reverse: swap children
	if n.lz.rev {
		n.l, n.r = n.r, n.l
	}
	// propagate tag to children (updates their val/agg immediately)
	if n.l != nil {
		t.applyAt(n.l, n.lz)
	}
	if n.r != nil {
		t.applyAt(n.r, n.lz)
	}
	n.lz = BeatsLazyIdentity()
}

// Recompute siz/agg from children + val
func (t *LazyBeatsTreap) pull(n *lazyBeatsTreapNode) {
	if n == nil {
		return
	}
	n.siz = 1 + t.siz(n.l) + t.siz(n.r)
	n.agg = t.agg(n.l).Plus(n.val).Plus(t.agg(n.r))
}

// merge assumes all indices in 'a' come before all indices in 'b'
func (t *LazyBeatsTreap) merge(a, b *lazyBeatsTreapNode) *lazyBeatsTreapNode {
	if a == nil {
		return b
	}
	if b == nil {
		return a
	}
	t.push(a)
	t.push(b)
	if a.pri > b.pri {
		a.r = t.merge(a.r, b)
		t.pull(a)
		return a
	}
	b.l = t.merge(a, b.l)
	t.pull(b)
	return b
}

// split by index: [0, k) and [k..)
func (t *LazyBeatsTreap) split(n *lazyBeatsTreapNode, k int) (*lazyBeatsTreapNode, *lazyBeatsTreapNode) {
	if n == nil {
		return nil, nil
	}
	t.push(n)
	if k <= t.siz(n.l) {
		a, b := t.split(n.l, k)
		n.l = b
		t.pull(n)
		return a, n
	}
	a, b := t.split(n.r, k-t.siz(n.l)-1)
	n.r = a
	t.pull(n)
	return n, b
}

func (t *LazyBeatsTreap) merge3(a, b, c *lazyBeatsTreapNode) *lazyBeatsTreapNode {
	return t.merge(t.merge(a, b), c)
}

// O(1) reset that leaks resources (like NodePool reset_keep_memory)
func (t *LazyBeatsTreap) ResetFast() { t.root = nil }

// real delete-all (safe for multitests)
func (t *LazyBeatsTreap) Clear() { t.root = nil }

// find i-th node (0-index). pushes lazies along path for correctness
func (t *LazyBeatsTreap) findi(i int) *lazyBeatsTreapNode {
	if !(0 <= i && i < t.Size()) {
		panic("findi: invalid index")
	}
	cur := t.root
	for {
		t.push(cur)
		L := t.siz(cur.l)
		if i == L {
			return cur
		}
		if i < L {
			cur = cur.l
		} else {
			i -= L + 1
			cur = cur.r
		}
	}
}

// point get (as Value)
func (t *LazyBeatsTreap) GetValue(i int) BeatsValue { return t.findi(i).val }

func (t *LazyBeatsTreap) SetInt64(i int, v int64) {
	t.Modifyi(i, func(n *lazyBeatsTreapNode) {
		n.val = MakeBeatsValue1(v)
	})
}

// modify i-th node with a callback op(node)
func (t *LazyBeatsTreap) Modifyi(i int, op func(*lazyBeatsTreapNode)) {
	if !(0 <= i && i < t.Size()) {
		panic("Modifyi: invalid index")
	}
	a, bc := t.split(t.root, i)
	b, c := t.split(bc, 1)
	// b is the single node
	t.push(b)
	op(b)
	t.pull(b)
	t.root = t.merge3(a, b, c)
}

// Beats apply on a whole subtree
// if can_break => nothing changes
// if can_tag => apply lazily at this node in O(1) and stop
// Else => recurse to children, then fix aggregates on the way back up
func (t *LazyBeatsTreap) applyBeats(n *lazyBeatsTreapNode, tag BeatsLazy) {
	if n == nil {
		return
	}
	t.push(n)
	if n.agg.CanBreak(tag) {
		return
	}
	if n.agg.CanTag(tag) {
		t.applyAt(n, tag)
		return
	}
	t.applyBeats(n.l, tag)
	t.applyBeats(n.r, tag)
	n.val.Apply(tag, 1)
	t.pull(n)
}

// In-place beats apply on a subrange [lo, hi) within this subtree
// lo/hi are relative indices inside n
func (t *LazyBeatsTreap) applyi(n *lazyBeatsTreapNode, lo, hi int, z BeatsLazy) {
	if n == nil || lo >= t.siz(n) || hi <= 0 {
		return
	}
	if n.agg.CanBreak(z) {
		return
	}

	if lo <= 0 && t.siz(n) <= hi && n.agg.CanTag(z) {
		t.applyAt(n, z)
		return
	}

	t.push(n)
	L := t.siz(n.l)
	if lo <= L && L < hi {
		n.val.Apply(z, 1)
	}

	t.applyi(n.l, lo, minInt(hi, L), z)
	t.applyi(n.r, lo-(L+1), hi-(L+1), z)
	t.pull(n)
}

func (t *LazyBeatsTreap) queryi(n *lazyBeatsTreapNode, lo, hi int) BeatsValue {
	if n == nil || lo >= t.siz(n) || hi <= 0 {
		return BeatsValueIdentity()
	}
	if lo <= 0 && t.siz(n) <= hi {
		return n.agg
	}
	t.push(n)
	L := t.siz(n.l)
	res := BeatsValueIdentity()
	res = res.Plus(t.queryi(n.l, lo, minInt(hi, L)))
	if lo <= L && L < hi {
		res = res.Plus(n.val)
	}
	res = res.Plus(t.queryi(n.r, lo-(L+1), hi-(L+1)))
	return res
}

func (t *LazyBeatsTreap) Insert(i int, x int64) {
	// Node made from scalar x
	nd := &lazyBeatsTreapNode{val: MakeBeatsValue1(x), agg: MakeBeatsValue1(x), lz: BeatsLazyIdentity(), siz: 1, pri: t.rng.Next()}
	a, b := t.split(t.root, i)
	t.root = t.merge(t.merge(a, nd), b)
}

// erase index i, returns erased Value
func (t *LazyBeatsTreap) Erase(i int) BeatsValue {
	if !(0 <= i && i < t.Size()) {
		panic("Erase: invalid index")
	}
	a, bc := t.split(t.root, i)
	b, c := t.split(bc, 1)
	t.push(b)
	ret := b.val
	t.root = t.merge(a, c)
	return ret
}

// rotate sequence so index i becomes start: [0..i) moves to end
func (t *LazyBeatsTreap) Rotate(i int) {
	if !(0 <= i && i <= t.Size()) {
		panic("Rotate: invalid index")
	}
	a, b := t.split(t.root, i)
	t.root = t.merge(b, a)
}

// reverse range [l, r)
func (t *LazyBeatsTreap) Reverse(l, r int) {
	a, bc := t.split(t.root, l)
	b, c := t.split(bc, r-l)
	tag := BeatsLazyIdentity()
	tag.rev = true
	if b != nil {
		b.lz.Compose(tag)
	}
	t.root = t.merge(a, t.merge(b, c))
}

// apply beats tag on range [l, r)
func (t *LazyBeatsTreap) Apply(l, r int, tag BeatsLazy) {
	a, bc := t.split(t.root, l)
	b, c := t.split(bc, r-l)
	t.applyBeats(b, tag)
	t.root = t.merge(a, t.merge(b, c))
}

// query aggregate on range [l, r) (split-based)
func (t *LazyBeatsTreap) Query(l, r int) BeatsValue {
	if !(0 <= l && l <= r && r <= t.Size()) {
		panic("Query: invalid range")
	}
	a, bc := t.split(t.root, l)
	b, c := t.split(bc, r-l)
	res := t.agg(b)
	t.root = t.merge(a, t.merge(b, c))
	return res
}

// query aggregate on range [l, r) (in-place, no splits)
func (t *LazyBeatsTreap) QueryFast(l, r int) BeatsValue {
	if !(0 <= l && l <= r && r <= t.Size()) {
		panic("QueryFast: invalid range")
	}
	return t.queryi(t.root, l, r)
}

func maxInt64(a, b int64) int64 {
	if a < b {
		return b
	}
	return a
}

func minInt64(a, b int64) int64 {
	if a < b {
		return a
	}
	return b
}

func minInt(a, b int) int {
	if a < b {
		return a
	}
	return b
}
