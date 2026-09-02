package splay

type SplayTreeActedMonoid[X any, A comparable] struct {
	n    int                    // number of input elements
	root int                    // root index
	t    []actedSplayNode[X, A] // node pool, 0 = null

	op  func(X, X) X
	idX func() X
	opA func(A, A) A
	idA func() A
	act func(X, A, int64) X
}

type actedSplayNode[X any, A comparable] struct {
	ch   [2]int
	val  X
	prod X
	siz  int
	p    int
	lz   A
}

func NewSplayTreeActedMonoid[X any, A comparable](
	n int,
	op func(X, X) X,
	idX func() X,
	opA func(A, A) A,
	idA func() A,
	act func(X, A, int64) X,
) *SplayTreeActedMonoid[X, A] {
	s := &SplayTreeActedMonoid[X, A]{op: op, idX: idX, opA: opA, idA: idA, act: act}
	s.Init(n + 5)
	s.Build(n, func(int) X { return idX() })
	return s
}

func NewSplayTreeActedMonoidFromSlice[X any, A comparable](
	v []X,
	op func(X, X) X,
	idX func() X,
	opA func(A, A) A,
	idA func() A,
	act func(X, A, int64) X,
) *SplayTreeActedMonoid[X, A] {
	s := &SplayTreeActedMonoid[X, A]{op: op, idX: idX, opA: opA, idA: idA, act: act}
	s.Init(len(v) + 5)
	s.Build(len(v), func(i int) X { return v[i] })
	return s
}

func (s *SplayTreeActedMonoid[X, A]) Init(reserveN int) {
	s.t = make([]actedSplayNode[X, A], 1, max(1, reserveN)+3) // dummy node at 0
	s.root = 0
	s.n = 0
}

// --- build ---
func (s *SplayTreeActedMonoid[X, A]) Build(m int, f func(int) X) {
	s.n = m
	a := make([]X, s.n+2)
	a[0] = s.idX() // left sentinel
	for i := 0; i < s.n; i++ {
		a[i+1] = f(i)
	}
	a[s.n+1] = s.idX() // right sentinel
	// rebuild pool
	s.t = s.t[:1]
	s.root = s.buildRec(a, 0, len(a), 0)
}

// --- basic ops ---
func (s *SplayTreeActedMonoid[X, A]) Size() int { return s.n }

// set a[pos] = x
func (s *SplayTreeActedMonoid[X, A]) Set(pos int, x X) {
	if !(0 <= pos && pos < s.n) {
		panic("Set: invalid index")
	}
	v := s.kth(pos + 1) // +1 cos of left sentinel
	s.splay(v, 0)
	s.push(v)
	s.t[v].val = x
	s.pull(v)
}

func (s *SplayTreeActedMonoid[X, A]) UpdateRange(l, r int, a A) {
	if !(0 <= l && l <= r && r <= s.n) {
		panic("UpdateRange: invalid range")
	}
	if l == r {
		return
	}
	L, R := s.expose(l, r)
	mid := s.t[R].ch[0]
	s.applyAt(mid, a)
	s.pull(R)
	s.pull(L)
}

func (s *SplayTreeActedMonoid[X, A]) Get(pos int) X {
	if !(0 <= pos && pos < s.n) {
		panic("Get: invalid index")
	}
	v := s.kth(pos + 1)
	s.splay(v, 0)
	s.push(v)
	return s.t[v].val
}

// prod on [l, r)
func (s *SplayTreeActedMonoid[X, A]) Prod(l, r int) X {
	if !(0 <= l && l <= r && r <= s.n) {
		panic("Prod: invalid range")
	}
	if l == r {
		return s.idX()
	}
	L, R := s.expose(l, r)
	mid := s.t[R].ch[0]
	if mid != 0 {
		return s.t[mid].prod
	}
	_ = L
	return s.idX()
}

func (s *SplayTreeActedMonoid[X, A]) ProdAll() X { return s.Prod(0, s.n) }

// insert x before position pos
func (s *SplayTreeActedMonoid[X, A]) Insert(pos int, x X) {
	if !(0 <= pos && pos <= s.n) {
		panic("Insert: invalid index")
	}
	L := s.kth(pos)
	s.splay(L, 0)
	R := s.kth(pos + 1)
	s.splay(R, L) // make R right child of root L

	nd := s.newNode(x, R)
	s.t[R].ch[0] = nd
	s.pull(R)
	s.pull(L)
	s.n++
}

// erase element at position pos (0..n-1)
func (s *SplayTreeActedMonoid[X, A]) Erase(pos int) {
	if !(0 <= pos && pos < s.n) {
		panic("Erase: invalid index")
	}
	L := s.kth(pos)
	s.splay(L, 0)
	R := s.kth(pos + 2)
	s.splay(R, L)

	s.t[R].ch[0] = 0 // drop middle (it has exactly 1 element)
	s.pull(R)
	s.pull(L)
	s.n--
}

func (s *SplayTreeActedMonoid[X, A]) MaxRight(check func(X) bool, l int) int {
	if !(0 <= l && l <= s.n) {
		panic("MaxRight: invalid index")
	}
	if !check(s.idX()) {
		panic("MaxRight: check(id) must be true")
	}
	if l == s.n {
		return s.n
	}
	L := s.kth(l)
	s.splay(L, 0)
	R := s.kth(s.n + 1)
	s.splay(R, L)
	mid := s.t[R].ch[0]

	sm := s.idX()
	length := s.walkPrefix(mid, &sm, check)
	s.pull(R)
	s.pull(L)
	return l + length
}

func (s *SplayTreeActedMonoid[X, A]) MinLeft(check func(X) bool, r int) int {
	if !(0 <= r && r <= s.n) {
		panic("MinLeft: invalid index")
	}
	if !check(s.idX()) {
		panic("MinLeft: check(id) must be true")
	}
	if r == 0 {
		return 0
	}
	L := s.kth(0)
	s.splay(L, 0)
	R := s.kth(r + 1)
	s.splay(R, L)
	mid := s.t[R].ch[0]

	sm := s.idX()
	length := s.walkSuffix(mid, &sm, check)
	s.pull(R)
	s.pull(L)
	return r - length
}

func (s *SplayTreeActedMonoid[X, A]) GetAll() []X {
	out := make([]X, 0, s.n)
	s.inorderCollect(s.root, &out)
	return out
}

// --- node utils ---
func (s *SplayTreeActedMonoid[X, A]) siz(x int) int {
	if x == 0 {
		return 0
	}
	return s.t[x].siz
}

// recompute parent from two children
func (s *SplayTreeActedMonoid[X, A]) pull(x int) {
	if x == 0 {
		return
	}
	s.t[x].siz = 1 + s.siz(s.t[x].ch[0]) + s.siz(s.t[x].ch[1])
	leftp := s.idX()
	if s.t[x].ch[0] != 0 {
		leftp = s.t[s.t[x].ch[0]].prod
	}
	rightp := s.idX()
	if s.t[x].ch[1] != 0 {
		rightp = s.t[s.t[x].ch[1]].prod
	}
	s.t[x].prod = s.op(leftp, s.op(s.t[x].val, rightp))
}

// update node's val/prod and store lazy tag
func (s *SplayTreeActedMonoid[X, A]) applyAt(x int, a A) {
	if x == 0 {
		return
	}
	s.t[x].val = s.act(s.t[x].val, a, 1)
	s.t[x].prod = s.act(s.t[x].prod, a, int64(s.t[x].siz))
	s.t[x].lz = s.opA(s.t[x].lz, a)
}

// apply lazy at children, clear lazy from this node
func (s *SplayTreeActedMonoid[X, A]) push(x int) {
	if x == 0 {
		return
	}
	if s.t[x].lz == s.idA() {
		return
	}
	s.applyAt(s.t[x].ch[0], s.t[x].lz)
	s.applyAt(s.t[x].ch[1], s.t[x].lz)
	s.t[x].lz = s.idA()
}

func (s *SplayTreeActedMonoid[X, A]) newNode(v X, p int) int {
	x := len(s.t)
	s.t = append(s.t, actedSplayNode[X, A]{val: v, prod: v, p: p, siz: 1, lz: s.idA()})
	return x
}

func (s *SplayTreeActedMonoid[X, A]) isRight(x int) bool {
	p := s.t[x].p
	return p != 0 && s.t[p].ch[1] == x
}

func (s *SplayTreeActedMonoid[X, A]) setChild(par, chi, dir int) {
	s.t[par].ch[dir] = chi
	if chi != 0 {
		s.t[chi].p = par
	}
}

func (s *SplayTreeActedMonoid[X, A]) rotate(x int) {
	p := s.t[x].p
	g := s.t[p].p
	dx := 0
	if s.isRight(x) {
		dx = 1
	}
	dp := 0
	if p != 0 && s.isRight(p) {
		dp = 1
	}
	b := s.t[x].ch[dx^1]

	s.setChild(p, b, dx)
	s.setChild(x, p, dx^1)

	s.t[x].p = g
	if g != 0 {
		s.t[g].ch[dp] = x
	} else {
		s.root = x
	}

	s.pull(p)
	s.pull(x)
}

// zig / zig-zig / zig-zag
func (s *SplayTreeActedMonoid[X, A]) splay(x, goal int) {
	st := make([]int, 0)
	for y := x; y != goal; y = s.t[y].p {
		st = append(st, y)
	}
	st = append(st, goal)
	for i := len(st) - 1; i >= 0; i-- {
		s.push(st[i])
	}
	for s.t[x].p != goal {
		p := s.t[x].p
		g := s.t[p].p
		if g == goal {
			s.rotate(x) // ZIG
		} else if s.isRight(x) == s.isRight(p) {
			s.rotate(p)
			s.rotate(x)
		} else {
			s.rotate(x)
			s.rotate(x)
		}
	}
	if goal == 0 {
		s.root = x
	}
}

// k-th in full sequence (including 2 sentinels), k in [0, n+1]
func (s *SplayTreeActedMonoid[X, A]) kth(k int) int {
	if !(0 <= k && k <= s.n+1) {
		panic("kth: invalid index")
	}
	x := s.root
	for {
		s.push(x)
		lsz := s.siz(s.t[x].ch[0])
		if k < lsz {
			x = s.t[x].ch[0]
		} else if k == lsz {
			return x
		} else {
			k -= lsz + 1
			x = s.t[x].ch[1]
		}
	}
}

// expose [l, r) user range:
// splay kth(l) to root, then splay kth(r+1) to root.right
func (s *SplayTreeActedMonoid[X, A]) expose(l, r int) (int, int) {
	L := s.kth(l)
	s.splay(L, 0)
	R := s.kth(r + 1)
	s.splay(R, L)
	return L, R
}

func (s *SplayTreeActedMonoid[X, A]) buildRec(a []X, l, r, p int) int {
	if l >= r {
		return 0
	}
	m := (l + r) >> 1
	x := s.newNode(a[m], p)
	lc := s.buildRec(a, l, m, x)
	rc := s.buildRec(a, m+1, r, x)
	s.t[x].ch[0] = lc
	s.t[x].ch[1] = rc
	s.pull(x)
	return x
}

func (s *SplayTreeActedMonoid[X, A]) walkPrefix(x int, sm *X, check func(X) bool) int {
	if x == 0 {
		return 0
	}
	s.push(x)
	res := 0
	L, R := s.t[x].ch[0], s.t[x].ch[1]

	if L != 0 {
		if !check(s.op(*sm, s.t[L].prod)) {
			return s.walkPrefix(L, sm, check)
		}
		*sm = s.op(*sm, s.t[L].prod)
		res += s.t[L].siz
	}
	if !check(s.op(*sm, s.t[x].val)) {
		return res
	}
	*sm = s.op(*sm, s.t[x].val)
	res++

	if R != 0 {
		if !check(s.op(*sm, s.t[R].prod)) {
			return res + s.walkPrefix(R, sm, check)
		}
		*sm = s.op(*sm, s.t[R].prod)
		res += s.t[R].siz
	}
	return res
}

func (s *SplayTreeActedMonoid[X, A]) walkSuffix(x int, sm *X, check func(X) bool) int {
	if x == 0 {
		return 0
	}
	s.push(x)
	res := 0
	L, R := s.t[x].ch[0], s.t[x].ch[1]

	if R != 0 {
		if !check(s.op(*sm, s.t[R].prod)) {
			return res + s.walkSuffix(R, sm, check)
		}
		*sm = s.op(*sm, s.t[R].prod)
		res += s.t[R].siz
	}
	if !check(s.op(*sm, s.t[x].val)) {
		return res
	}
	*sm = s.op(*sm, s.t[x].val)
	res++

	if L != 0 {
		if !check(s.op(*sm, s.t[L].prod)) {
			return s.walkSuffix(L, sm, check)
		}
		*sm = s.op(*sm, s.t[L].prod)
		res += s.t[L].siz
	}
	return res
}

func (s *SplayTreeActedMonoid[X, A]) inorderCollect(x int, out *[]X) {
	if x == 0 {
		return
	}
	s.push(x)
	s.inorderCollect(s.t[x].ch[0], out)
	*out = append(*out, s.t[x].val)
	s.inorderCollect(s.t[x].ch[1], out)
	// if called on root, drop sentinels
	if x == s.root && len(*out) >= 2 {
		*out = (*out)[1 : len(*out)-1]
	}
}
