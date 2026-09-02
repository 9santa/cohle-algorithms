package segtree

// Recursive top-to-bottom
type SegmentTreeRecursive struct {
	n    int // Size of the input array
	size int // Smallest power of 2 >= n
	tree []int64
}

func NewSegmentTreeRecursive(arr []int) *SegmentTreeRecursive {
	st := &SegmentTreeRecursive{n: len(arr)}
	if st.n == 0 {
		st.size = 1
		st.tree = make([]int64, 2)
		return st
	}

	st.size = 1
	for st.size < st.n {
		st.size *= 2
	}
	st.tree = make([]int64, 2*st.size)

	// Initialize leaves (source array elements)
	for i := 0; i < st.n; i++ {
		st.tree[st.size+i-1] = int64(arr[i])
	}

	// Internal nodes (for sum)
	for i := st.size - 2; i >= 0; i-- {
		st.tree[i] = st.tree[2*i+1] + st.tree[2*i+2]
	}
	return st
}

// Set value at index i (in the source array)
func (st *SegmentTreeRecursive) set(i int, val int64, x, xl, xr int) {
	if xl == xr-1 { // leaf node
		st.tree[x] = val
		return
	}

	xm := (xl + xr) / 2
	if i < xm {
		st.set(i, val, 2*x+1, xl, xm) // process left child subtree
	} else {
		st.set(i, val, 2*x+2, xm, xr) // process right child subtree
	}

	st.tree[x] = st.tree[2*x+1] + st.tree[2*x+2] // update parent (root of the subtree)
}

// Sum query in range [l, r)
func (st *SegmentTreeRecursive) sumQuery(l, r, x, xl, xr int) int64 {
	if xl > r || xr < l {
		return 0 // no overlap
	}
	if xl >= l && xr <= r {
		return st.tree[x] // full overlap
	}
	xm := (xl + xr) / 2
	suml := st.sumQuery(l, r, 2*x+1, xl, xm)
	sumr := st.sumQuery(l, r, 2*x+2, xm, xr)
	return suml + sumr
}

func (st *SegmentTreeRecursive) Set(i int, val int64) {
	if i < 0 || i >= st.n {
		return // out of bounds
	}
	st.set(i, val, 0, 0, st.size)
}

func (st *SegmentTreeRecursive) Sum(l, r int) int64 {
	if l < 0 || r > st.n || l > r {
		return 0 // correct half-interval check
	}
	return st.sumQuery(l, r, 0, 0, st.size)
}

// Searching for the first element greater than a given 'x' and a given range [l, r]
func (st *SegmentTreeRecursive) GetFirst(v, tl, tr, l, r, x int) int {
	if tl > r || tr < l {
		return -1
	}
	if st.tree[v] <= int64(x) {
		return -1
	}
	if tl == tr {
		return tl
	}
	tm := tl + (tr-tl)/2
	left := st.GetFirst(2*v, tl, tm, l, r, x)
	if left != -1 {
		return left
	}
	return st.GetFirst(2*v+1, tm+1, tr, l, r, x)
}

// Iterative bottom-to-top
type SegmentTreeIterative struct {
	n    int
	size int
	tree []int64
}

func NewSegmentTreeIterative(arr []int) *SegmentTreeIterative {
	st := &SegmentTreeIterative{n: len(arr), size: 1}
	if st.n == 0 {
		st.tree = make([]int64, 2)
		return st
	}
	for st.size < st.n {
		st.size *= 2
	}
	st.tree = make([]int64, 2*st.size)
	for i := 0; i < st.n; i++ {
		st.tree[st.size+i] = int64(arr[i])
	}
	for i := st.size - 1; i > 0; i-- {
		st.tree[i] = st.tree[2*i] + st.tree[2*i+1]
	}
	return st
}

func (st *SegmentTreeIterative) Set(i int, val int64) {
	x := i + st.size
	st.tree[x] = val // update value at the correct index
	// climb up
	for x /= 2; x > 0; x /= 2 {
		st.tree[x] = st.tree[2*x] + st.tree[2*x+1] // update all of the needed parents
	}
}

// [l, r)
func (st *SegmentTreeIterative) Sum(l, r int) int64 {
	l += st.size // included
	r += st.size // not included
	var ans int64
	for l < r {
		if l%2 == 1 {
			ans += st.tree[l]
			l++
		}
		if r%2 == 1 {
			r--
			ans += st.tree[r]
		}
		l /= 2
		r /= 2
	}
	return ans
}

// Segment Tree With A Proper Tree Structure
type ProperNode struct {
	Val         int64
	Left, Right *ProperNode
	L, R        int // segment range [l, r)
}

type SegmentTreeProper struct {
	Root *ProperNode
}

func NewSegmentTreeProper(arr []int) *SegmentTreeProper {
	return &SegmentTreeProper{Root: buildProper(arr, 0, len(arr))}
}

func buildProper(arr []int, l, r int) *ProperNode {
	node := &ProperNode{L: l, R: r}
	if r-l == 1 {
		if l < len(arr) {
			node.Val = int64(arr[l])
		}
		return node
	}
	m := l + (r-l)/2
	node.Left = buildProper(arr, l, m)
	node.Right = buildProper(arr, m, r)
	node.Val = node.Left.Val + node.Right.Val
	return node
}

func setProper(node *ProperNode, i int, val int64) {
	if node.R-node.L == 1 {
		node.Val = val
		return
	}
	m := node.L + (node.R-node.L)/2
	if i < m {
		setProper(node.Left, i, val)
	} else {
		setProper(node.Right, i, val)
	}
	node.Val = node.Left.Val + node.Right.Val
}

// [ql, qr)
func sumProper(node *ProperNode, xl, xr int) int64 {
	if node == nil || xl >= node.R || xr <= node.L {
		return 0
	}
	if xl <= node.L && xr >= node.R {
		return node.Val
	}
	return sumProper(node.Left, xl, xr) + sumProper(node.Right, xl, xr)
}

func ConvertToArray(node *ProperNode, tree *[]int64, i int) {
	if node == nil {
		return
	}
	if i >= len(*tree) {
		newTree := make([]int64, i+1)
		copy(newTree, *tree)
		*tree = newTree
	}
	(*tree)[i] = node.Val
	ConvertToArray(node.Left, tree, 2*i+1)
	ConvertToArray(node.Right, tree, 2*i+2)
}

func (st *SegmentTreeProper) Set(i int, val int64) {
	setProper(st.Root, i, val)
}

func (st *SegmentTreeProper) Sum(l, r int) int64 {
	return sumProper(st.Root, l, r)
}
