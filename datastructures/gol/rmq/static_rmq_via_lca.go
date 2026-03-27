package rmq

// --- Static RMQ on array via LCA ---
// array A -> Cartesian tree -> LCA
// LCA -> RMQ on Euler-depth array (adj diff +-1)
// +-1 RMQ via Four Russians OR mask[i] approach -> O(1) queries
type FastRMQ struct {
	n int
	a []int64

	// Cartesian tree
	parent []int
	lc     []int // lc - left child
	rc     []int // rc - right child
	root   int

	// Euler rout for LCA
	euler []int // node ids
	depth []int // depth at each euler position
	first []int // first occ of each node in euler

	rmqDepth *PlusMinusOneRMQ // RMQ over depth[]
}

func NewFastRMQ(arr []int64) *FastRMQ {
	r := &FastRMQ{root: -1}
	r.Build(arr)
	return r
}

func (r *FastRMQ) keyLess(i, j int) bool {
	if r.a[i] != r.a[j] {
		return r.a[i] < r.a[j]
	}
	return i < j
}

func (r *FastRMQ) buildCartesian() {
	r.parent = make([]int, r.n)
	r.lc = make([]int, r.n)
	r.rc = make([]int, r.n)
	for i := 0; i < r.n; i++ {
		r.parent[i] = -1
		r.lc[i] = -1
		r.rc[i] = -1
	}

	st := make([]int, 0, r.n)
	for i := 0; i < r.n; i++ {
		last := -1
		for len(st) > 0 && r.keyLess(i, st[len(st)-1]) {
			last = st[len(st)-1]
			st = st[:len(st)-1]
		}
		if len(st) > 0 {
			r.parent[i] = st[len(st)-1]
			r.rc[st[len(st)-1]] = i
		}
		if last != -1 {
			r.parent[last] = i
			r.lc[i] = last
		}
		st = append(st, i)
	}

	r.root = -1
	for i := 0; i < r.n; i++ {
		if r.parent[i] == -1 {
			r.root = i
			break
		}
	}
}

func (r *FastRMQ) buildEulerTour() {
	r.euler = r.euler[:0]
	r.depth = r.depth[:0]
	r.first = make([]int, r.n)
	for i := range r.first {
		r.first[i] = -1
	}

	r.euler = make([]int, 0, 2*r.n-1)
	r.depth = make([]int, 0, 2*r.n-1)

	// build children lists
	kids := make([][2]int, r.n)
	kidCnt := make([]int, r.n)
	for v := 0; v < r.n; v++ {
		if r.lc[v] != -1 {
			kids[v][kidCnt[v]] = r.lc[v]
			kidCnt[v]++
		}
		if r.rc[v] != -1 {
			kids[v][kidCnt[v]] = r.rc[v]
			kidCnt[v]++
		}
	}

	record := func(v, d int) {
		if r.first[v] == -1 {
			r.first[v] = len(r.euler)
		}
		r.euler = append(r.euler, v)
		r.depth = append(r.depth, d)
	}

	// iterative DFS for Euler tour
	stV := make([]int, 0, r.n)
	stI := make([]int, 0, r.n)
	stD := make([]int, 0, r.n)
	stV = append(stV, r.root)
	stI = append(stI, 0)
	stD = append(stD, 0)
	record(r.root, 0)

	for len(stV) > 0 {
		v := stV[len(stV)-1]
		i := stI[len(stI)-1]
		d := stD[len(stD)-1]

		if i < kidCnt[v] {
			c := kids[v][i]
			stI[len(stI)-1]++
			stV = append(stV, c)
			stI = append(stI, 0)
			stD = append(stD, d+1)
			record(c, d+1)
		} else {
			stV = stV[:len(stV)-1]
			stI = stI[:len(stI)-1]
			stD = stD[:len(stD)-1]
			if len(stV) > 0 {
				record(stV[len(stV)-1], stD[len(stD)-1])
			}
		}
	}
}

func (r *FastRMQ) Build(arr []int64) {
	r.a = append([]int64(nil), arr...)
	r.n = len(r.a)
	if r.n == 0 {
		r.root = -1
		r.rmqDepth = nil
		return
	}
	r.buildCartesian()
	r.buildEulerTour()
	r.rmqDepth = NewPlusMinusOneRMQ(r.depth)
}

func (r *FastRMQ) LCAIndex(u, v int) int {
	L := r.first[u]
	R := r.first[v]
	pos := r.rmqDepth.Query(L, R)
	return r.euler[pos]
}

func (r *FastRMQ) RMQIndex(l, rr int) int {
	if l > rr {
		l, rr = rr, l
	}
	return r.LCAIndex(l, rr)
}

func (r *FastRMQ) RMQValue(l, rr int) int64 {
	return r.a[r.RMQIndex(l, rr)]
}
