package splay

type SplayTreeMonoid[T any] struct {
	n    int            // number of input elements
	root int            // root index
	t    []splayNode[T] // node pool, 0 = null
	op   func(T, T) T
	id   func() T
}

type splayNode[T any] struct {
	ch   [2]int
	val  T
	prod T
	siz  int
	p    int
}

func NewSplayTreeMonoid[T any](n int, op func(T, T) T, id func() T) *SplayTreeMonoid[T] {
	s := &SplayTreeMonoid[T]{op: op, id: id}
	s.Init(n + 5)
	s.Build(n, func(int) T { return id() })
	return s
}

func NewSplayTreeMonoidFromSlice[T any](v []T, op func(T, T) T, id func() T) *SplayTreeMonoid[T] {
	s := &SplayTreeMonoid[T]{op: op, id: id}
	s.Init(len(v) + 5)
	s.Build(len(v), func(i int) T { return v[i] })
	return s
}

func (s *SplayTreeMonoid[T]) Init(reserveN int) {
	s.t = make([]splayNode[T], 1, max(1, reserveN)+3) // dummy node at 0
	s.root = 0
	s.n = 0
}

// --- build ---
func (s *SplayTreeMonoid[T]) Build(m int, f func(int) T) {
	s.n = m
	a := make([]T, s.n+2)
	a[0] = s.id() // left sentinel
	for i := 0; i < s.n; i++ {
		a[i+1] = f(i)
	}
	a[s.n+1] = s.id() // right sentinel
	// rebuild pool
	s.t = s.t[:1]
	s.root = s.buildRec(a, 0, len(a), 0)
}

// --- basic ops ---
func (s *SplayTreeMonoid[T]) Size() int { return s.n }

// set a[pos] = x
func (s *SplayTreeMonoid[T]) Set(pos int, x T) {
	if !(0 <= pos && pos < s.n) {
		panic("Set: invalid index")
	}
	v := s.kth(pos + 1) // +1 cos of left sentinel
	s.splay(v, 0)
	s.t[v].val = x
	s.pull(v)
}

func (s *SplayTreeMonoid[T]) Get(pos int) T {
	if !(0 <= pos && pos < s.n) {
		panic("Get: invalid index")
	}
	v := s.kth(pos + 1)
	s.splay(v, 0)
	return s.t[v].val
}

// prod on [l, r)
func (s *SplayTreeMonoid[T]) Prod(l, r int) T {
	if !(0 <= l && l <= r && r <= s.n) {
		panic("Prod: invalid range")
	}
	if l == r {
		return s.id()
	}
	_, R := s.expose(l, r)
	mid := s.t[R].ch[0]
	if mid != 0 {
		return s.t[mid].prod
	}
	return s.id()
}

func (s *SplayTreeMonoid[T]) ProdAll() T { return s.Prod(0, s.n) }

// insert x before position pos
func (s *SplayTreeMonoid[T]) Insert(pos int, x T) {
	if !(0 <= pos && pos <= s.n) {
		panic("Insert: invalid index")
	}
	// boundaries are pos and pos+1 cos of sentinel
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
func (s *SplayTreeMonoid[T]) Erase(pos int) {
	if !(0 <= pos && pos < s.n) {
		panic("Erase: invalid index")
	}
	// isolate [pos, pos+1)
	L := s.kth(pos)
	s.splay(L, 0)
	R := s.kth(pos + 2)
	s.splay(R, L)

	s.t[R].ch[0] = 0 // drop middle (it has exactly 1 element)
	s.pull(R)
	s.pull(L)
	s.n--
}

// max_right: largest r >= l such that check(prod(l, r)) is true
func (s *SplayTreeMonoid[T]) MaxRight(check func(T) bool, l int) int {
	if !(0 <= l && l <= s.n) {
		panic("MaxRight: invalid index")
	}
	if !check(s.id()) {
		panic("MaxRight: check(id) must be true")
	}
	if l == s.n {
		return s.n
	}
	// isolate suffix [l, n)
	L := s.kth(l)
	s.splay(L, 0)
	R := s.kth(s.n + 1)
	s.splay(R, L)
	mid := s.t[R].ch[0]

	sm := s.id()
	length := s.walkPrefix(mid, &sm, check)
	s.pull(R)
	s.pull(L)
	return l + length
}

// min_left: smallest l <= r such that check(prod(l, r)) is true
func (s *SplayTreeMonoid[T]) MinLeft(check func(T) bool, r int) int {
	if !(0 <= r && r <= s.n) {
		panic("MinLeft: invalid index")
	}
	if !check(s.id()) {
		panic("MinLeft: check(id) must be true")
	}
	if r == 0 {
		return 0
	}
	// isolate prefix [0, r)
	L := s.kth(0)
	s.splay(L, 0)
	R := s.kth(r + 1)
	s.splay(R, L)
	mid := s.t[R].ch[0]

	sm := s.id()
	length := s.walkSuffix(mid, &sm, check)
	s.pull(R)
	s.pull(L)
	return r - length
}

// collect everything in correct order with in-order traversal
func (s *SplayTreeMonoid[T]) GetAll() []T {
	out := make([]T, 0, s.n)
	s.inorderCollect(s.root, &out)
	return out
}

// --- node utils ---
func (s *SplayTreeMonoid[T]) siz(x int) int {
	if x == 0 {
		return 0
	}
	return s.t[x].siz
}

func (s *SplayTreeMonoid[T]) pull(x int) {
	if x == 0 {
		return
	}
	s.t[x].siz = 1 + s.siz(s.t[x].ch[0]) + s.siz(s.t[x].ch[1])
	leftp := s.id()
	if s.t[x].ch[0] != 0 {
		leftp = s.t[s.t[x].ch[0]].prod
	}
	rightp := s.id()
	if s.t[x].ch[1] != 0 {
		rightp = s.t[s.t[x].ch[1]].prod
	}
	s.t[x].prod = s.op(leftp, s.op(s.t[x].val, rightp))
}

func (s *SplayTreeMonoid[T]) newNode(v T, p int) int {
	x := len(s.t)
	s.t = append(s.t, splayNode[T]{val: v, prod: v, p: p, siz: 1})
	return x
}

func (s *SplayTreeMonoid[T]) isRight(x int) bool {
	p := s.t[x].p
	return p != 0 && s.t[p].ch[1] == x
}

func (s *SplayTreeMonoid[T]) setChild(par, chi, dir int) {
	s.t[par].ch[dir] = chi
	if chi != 0 {
		s.t[chi].p = par
	}
}

func (s *SplayTreeMonoid[T]) rotate(x int) {
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
func (s *SplayTreeMonoid[T]) splay(x, goal int) {
	for s.t[x].p != goal {
		p := s.t[x].p
		g := s.t[p].p
		if g == goal {
			s.rotate(x) // ZIG
		} else if s.isRight(x) == s.isRight(p) {
			// ZIG-ZIG
			s.rotate(p)
			s.rotate(x)
		} else {
			// ZIG-ZAG
			s.rotate(x)
			s.rotate(x)
		}
	}
	if goal == 0 {
		s.root = x
	}
}

// k-th in full sequence (including 2 sentinels), k in [0, n+1]
func (s *SplayTreeMonoid[T]) kth(k int) int {
	if !(0 <= k && k <= s.n+1) {
		panic("kth: invalid index")
	}
	x := s.root
	for {
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
func (s *SplayTreeMonoid[T]) expose(l, r int) (int, int) {
	L := s.kth(l)
	s.splay(L, 0)
	R := s.kth(r + 1)
	s.splay(R, L)
	return L, R
}

// recursive build
func (s *SplayTreeMonoid[T]) buildRec(a []T, l, r, p int) int {
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

func (s *SplayTreeMonoid[T]) walkPrefix(x int, sm *T, check func(T) bool) int {
	if x == 0 {
		return 0
	}
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

func (s *SplayTreeMonoid[T]) walkSuffix(x int, sm *T, check func(T) bool) int {
	if x == 0 {
		return 0
	}
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

func (s *SplayTreeMonoid[T]) inorderCollect(x int, out *[]T) {
	if x == 0 {
		return
	}
	s.inorderCollect(s.t[x].ch[0], out)
	*out = append(*out, s.t[x].val)
	s.inorderCollect(s.t[x].ch[1], out)
	// if called on root, drop sentinels
	if x == s.root && len(*out) >= 2 {
		*out = (*out)[1 : len(*out)-1]
	}
}

func max(a, b int) int {
	if a < b {
		return b
	}
	return a
}
