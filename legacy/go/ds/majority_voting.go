package gol

import "sort"

// O((n+q) log n)
type MajorityDS struct {
	a   []int
	pos map[int][]int
	st  majoritySegTree
}

type majorityNode struct {
	cand int // possible majority candidate
	cnt  int // net dominance after cancellations
}

func combineMajority(a, b majorityNode) majorityNode {
	if a.cnt == 0 {
		return b
	}
	if b.cnt == 0 {
		return a
	}
	if a.cand == b.cand {
		return majorityNode{cand: a.cand, cnt: a.cnt + b.cnt}
	}
	if a.cnt > b.cnt {
		return majorityNode{cand: a.cand, cnt: a.cnt - b.cnt}
	}
	if b.cnt > a.cnt {
		return majorityNode{cand: b.cand, cnt: b.cnt - a.cnt}
	}
	return majorityNode{}
}

type majoritySegTree struct {
	n int
	t []majorityNode
}

func (s *majoritySegTree) init(n int) {
	s.n = n
	s.t = make([]majorityNode, 4*n)
}

func (s *majoritySegTree) build(a []int, v, tl, tr int) {
	if tl == tr {
		s.t[v] = majorityNode{cand: a[tl], cnt: 1}
	} else {
		tm := tl + (tr-tl)/2
		s.build(a, v*2, tl, tm)
		s.build(a, v*2+1, tm+1, tr)
		s.t[v] = combineMajority(s.t[v*2], s.t[v*2+1])
	}
}

func (s *majoritySegTree) update(v, tl, tr, posi, val int) {
	if tl == tr {
		s.t[v] = majorityNode{cand: val, cnt: 1}
	} else {
		tm := tl + (tr-tl)/2
		if posi <= tm {
			s.update(v*2, tl, tm, posi, val)
		} else {
			s.update(v*2+1, tm+1, tr, posi, val)
		}
		s.t[v] = combineMajority(s.t[v*2], s.t[v*2+1])
	}
}

func (s *majoritySegTree) query(v, tl, tr, l, r int) majorityNode {
	if l > r {
		return majorityNode{}
	}
	if l == tl && r == tr {
		return s.t[v]
	}
	tm := tl + (tr-tl)/2
	return combineMajority(
		s.query(v*2, tl, tm, l, minInt(r, tm)),
		s.query(v*2+1, tm+1, tr, maxInt(l, tm+1), r),
	)
}

func NewMajorityDS(a []int) *MajorityDS {
	ds := &MajorityDS{
		a:   append([]int(nil), a...),
		pos: make(map[int][]int),
	}
	for i, v := range ds.a {
		ds.pos[v] = append(ds.pos[v], i)
	}
	ds.st.init(len(ds.a))
	if len(ds.a) > 0 {
		ds.st.build(ds.a, 1, 0, len(ds.a)-1)
	}
	return ds
}

func (ds *MajorityDS) Update(i, val int) {
	old := ds.a[i]
	if old == val {
		return
	}

	oldVec := ds.pos[old]
	oldPos := sort.SearchInts(oldVec, i)
	if oldPos < len(oldVec) && oldVec[oldPos] == i {
		oldVec = append(oldVec[:oldPos], oldVec[oldPos+1:]...)
	}
	if len(oldVec) == 0 {
		delete(ds.pos, old)
	} else {
		ds.pos[old] = oldVec
	}

	newVec := ds.pos[val]
	insertPos := sort.SearchInts(newVec, i)
	newVec = append(newVec, 0)
	copy(newVec[insertPos+1:], newVec[insertPos:])
	newVec[insertPos] = i
	ds.pos[val] = newVec

	ds.a[i] = val
	ds.st.update(1, 0, len(ds.a)-1, i, val)
}

// Query returns the majority element on [l, r), if it exists.
func (ds *MajorityDS) Query(l, r int) (int, bool) {
	if !(0 <= l && l <= r && r <= len(ds.a)) {
		panic("Query: invalid range")
	}
	if l == r {
		return 0, false
	}
	res := ds.st.query(1, 0, len(ds.a)-1, l, r-1)
	if res.cnt == 0 {
		return 0, false
	}

	v := res.cand
	freq := ds.count(v, l, r)
	if freq*2 > (r - l) {
		return v, true
	}
	return 0, false
}

func (ds *MajorityDS) count(v, l, r int) int {
	vec := ds.pos[v]
	return sort.SearchInts(vec, r) - sort.SearchInts(vec, l)
}

// SLOW VERSION! FOR LEARNING PURPOSES! vector.erase() is the killer here, need something better
// For example: Fenwick over Fenwick01
type SlowMajorityDS struct {
	a   []int
	pos map[int][]int
	st  majoritySegTree
}

func NewSlowMajorityDS(a []int) *SlowMajorityDS {
	ds := &SlowMajorityDS{
		a:   append([]int(nil), a...),
		pos: make(map[int][]int),
	}
	for i, v := range ds.a {
		ds.pos[v] = append(ds.pos[v], i)
	}
	ds.st.init(len(ds.a))
	if len(ds.a) > 0 {
		ds.st.build(ds.a, 1, 0, len(ds.a)-1)
	}
	return ds
}

func (ds *SlowMajorityDS) Update(i, val int) {
	old := ds.a[i]
	oldVec := ds.pos[old]
	for j, x := range oldVec {
		if x == i {
			ds.pos[old] = append(oldVec[:j], oldVec[j+1:]...)
			break
		}
	}
	ds.pos[val] = append(ds.pos[val], i)
	sort.Ints(ds.pos[val])
	ds.a[i] = val
	ds.st.update(1, 0, len(ds.a)-1, i, val)
}

func (ds *SlowMajorityDS) Count(v, l, r int) int {
	vec := ds.pos[v]
	return sort.SearchInts(vec, r) - sort.SearchInts(vec, l)
}

func (ds *SlowMajorityDS) Query(l, r int) (int, bool) {
	if l >= r {
		return 0, false
	}
	res := ds.st.query(1, 0, len(ds.a)-1, l, r-1)
	if res.cnt == 0 {
		return 0, false
	}
	freq := ds.Count(res.cand, l, r)
	if freq*2 > (r - l) {
		return res.cand, true
	}
	return 0, false
}
