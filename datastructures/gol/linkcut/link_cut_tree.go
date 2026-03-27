package linkcut

type LinkCut struct {
	n int
	t []linkCutNode
}

type linkCutNode struct {
	ch  [2]int // splay children
	p   int    // parent
	siz int    // subtree size
	rev bool   // lazy reverse tag

	val int64 // vertex value
	sum int64 // sum on aux splay subtree
	add int64 // lazy add on aux splay subtree
}

func NewLinkCut(n int) *LinkCut {
	lc := &LinkCut{}
	lc.Init(n)
	return lc
}

func NewLinkCutFromSlice(a []int64) *LinkCut {
	lc := NewLinkCut(len(a) - 1) // expect 1-indexed a[1..n]
	for i := 1; i <= lc.n; i++ {
		lc.t[i].val = a[i]
		lc.t[i].sum = a[i]
	}
	return lc
}

func (lc *LinkCut) Init(n int) {
	lc.n = n
	lc.t = make([]linkCutNode, n+1)
	for i := 1; i <= n; i++ {
		lc.t[i].siz = 1
		lc.t[i].sum = lc.t[i].val
	}
}

// ===== basic helpers =====

func (lc *LinkCut) isRoot(x int) bool {
	p := lc.t[x].p
	return p == 0 || (lc.t[p].ch[0] != x && lc.t[p].ch[1] != x)
}

func (lc *LinkCut) siz(x int) int {
	if x == 0 {
		return 0
	}
	return lc.t[x].siz
}

func (lc *LinkCut) sumVal(x int) int64 {
	if x == 0 {
		return 0
	}
	return lc.t[x].sum
}

// recalculate parent from its 2 children
func (lc *LinkCut) pull(x int) {
	if x == 0 {
		return
	}
	lc.t[x].siz = 1 + lc.siz(lc.t[x].ch[0]) + lc.siz(lc.t[x].ch[1])
	lc.t[x].sum = lc.t[x].val + lc.sumVal(lc.t[x].ch[0]) + lc.sumVal(lc.t[x].ch[1])
}

func (lc *LinkCut) applyRev(x int) {
	if x == 0 {
		return
	}
	lc.t[x].ch[0], lc.t[x].ch[1] = lc.t[x].ch[1], lc.t[x].ch[0]
	lc.t[x].rev = !lc.t[x].rev // invert rev flag
}

func (lc *LinkCut) applyAdd(x int, delta int64) {
	if x == 0 {
		return
	}
	lc.t[x].val += delta
	lc.t[x].sum += delta * int64(lc.t[x].siz)
	lc.t[x].add += delta
}

func (lc *LinkCut) push(x int) {
	if x == 0 {
		return
	}

	if lc.t[x].rev {
		lc.applyRev(lc.t[x].ch[0])
		lc.applyRev(lc.t[x].ch[1])
		lc.t[x].rev = false
	}

	if lc.t[x].add != 0 {
		lc.applyAdd(lc.t[x].ch[0], lc.t[x].add)
		lc.applyAdd(lc.t[x].ch[1], lc.t[x].add)
		lc.t[x].add = 0
	}
}

// push full path from root to x
func (lc *LinkCut) pushAll(x int) {
	stk := make([]int, 0, 32) // log(INT_MAX)
	y := x
	stk = append(stk, y)
	for !lc.isRoot(y) {
		y = lc.t[y].p
		stk = append(stk, y)
	}
	for i := len(stk) - 1; i >= 0; i-- {
		lc.push(stk[i])
	}
}

func (lc *LinkCut) rotate(x int) {
	p := lc.t[x].p
	g := lc.t[p].p
	dirx := 0
	if lc.t[p].ch[1] == x {
		dirx = 1 // 0 if x is left child of p, 1 if right child
	}
	b := lc.t[x].ch[dirx^1] // dirx is the opposite child, subtree of which will move

	if !lc.isRoot(p) {
		if lc.t[g].ch[0] == p {
			lc.t[g].ch[0] = x
		} else if lc.t[g].ch[1] == p {
			lc.t[g].ch[1] = x
		}
	}
	lc.t[x].p = g

	lc.t[x].ch[dirx^1] = p
	lc.t[p].p = x

	lc.t[p].ch[dirx] = b
	if b != 0 {
		lc.t[b].p = p
	}

	lc.pull(p)
	lc.pull(x)
}

func (lc *LinkCut) splay(x int) {
	lc.pushAll(x)
	for !lc.isRoot(x) {
		p := lc.t[x].p
		g := lc.t[p].p
		if !lc.isRoot(p) {
			zigzig := (lc.t[p].ch[0] == x) == (lc.t[g].ch[0] == p)
			if zigzig {
				lc.rotate(p)
			} else {
				lc.rotate(x)
			}
		}
		lc.rotate(x)
	}
}

// ===== Link Cut Tree core =====

// makes path (root ... x) preferred; x becomes root of its aux splay
func (lc *LinkCut) Expose(x int) int {
	last := 0
	for y := x; y != 0; y = lc.t[y].p {
		lc.splay(y)
		lc.t[y].ch[1] = last
		if last != 0 {
			lc.t[last].p = y
		}
		lc.pull(y)
		last = y
	}
	lc.splay(x)
	return last
}

func (lc *LinkCut) Access(x int) int {
	return lc.Expose(x)
}

func (lc *LinkCut) Evert(x int) {
	lc.Expose(x)
	lc.applyRev(x)
}

func (lc *LinkCut) Makeroot(x int) {
	lc.Evert(x)
}

func (lc *LinkCut) Findroot(x int) int {
	lc.Expose(x)
	for lc.t[x].ch[0] != 0 {
		lc.push(x)
		x = lc.t[x].ch[0]
	}
	lc.splay(x)
	return x
}

func (lc *LinkCut) Connected(u, v int) bool {
	if u == v {
		return true
	}
	return lc.Findroot(u) == lc.Findroot(v)
}

// after split(u, v), the whole path u-v is exactly the aux splay rooted at v
func (lc *LinkCut) Split(u, v int) bool {
	if !lc.Connected(u, v) {
		return false
	}
	lc.Evert(u)
	lc.Expose(v)
	return true
}

func (lc *LinkCut) Link(u, v int) bool {
	lc.Evert(u)
	if lc.Findroot(v) == u {
		return false // already in one tree
	}
	lc.t[u].p = v
	return true
}

func (lc *LinkCut) Cut(u, v int) bool {
	lc.Evert(u)
	lc.Expose(v)

	// if edge (u, v) exists directly, then u is left child of v
	// and u has no right child
	if lc.t[v].ch[0] != u || lc.t[u].ch[1] != 0 {
		return false
	}

	lc.t[v].ch[0] = 0
	lc.t[u].p = 0
	lc.pull(v)
	return true
}

// ===== values / queries =====

func (lc *LinkCut) GetValue(x int) int64 {
	lc.Expose(x)
	return lc.t[x].val
}

func (lc *LinkCut) SetValue(x int, val int64) {
	lc.Expose(x)
	lc.t[x].val = val
	lc.pull(x)
}

func (lc *LinkCut) UpdateValue(x int, delta int64) {
	lc.Expose(x)
	lc.t[x].val += delta
	lc.pull(x)
}

func (lc *LinkCut) PathAdd(u, v int, delta int64) bool {
	if !lc.Split(u, v) {
		return false
	}
	lc.applyAdd(v, delta)
	return true
}

func (lc *LinkCut) PathSum(u, v int) int64 {
	ok := lc.Split(u, v)
	if !ok {
		panic("PathSum: nodes are disconnected")
	}
	return lc.t[v].sum
}

// LCA for rooted interpretation of current represented tree
// works when u, v are connected
func (lc *LinkCut) LCA(u, v int) int {
	if !lc.Connected(u, v) {
		return 0
	}
	lc.Expose(u)
	return lc.Expose(v)
}
