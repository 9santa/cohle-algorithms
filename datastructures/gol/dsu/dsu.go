package dsu

// Disjoin-Set-Union
type DSU struct {
	parent     []uint64
	rank       []uint64
	setSize    []uint64
	maxElement []uint64
	minElement []uint64
	count      uint64 // number of disjoint sets
}

// Constructor
func NewDSU(size uint64) *DSU {
	d := &DSU{
		parent:     make([]uint64, size),
		rank:       make([]uint64, size),
		setSize:    make([]uint64, size),
		maxElement: make([]uint64, size),
		minElement: make([]uint64, size),
		count:      size,
	}

	// each node is its own parent at the start
	for i := uint64(0); i < size; i++ {
		d.parent[i] = i
		d.rank[i] = 0
		d.setSize[i] = 1
		d.maxElement[i] = i
		d.minElement[i] = i
	}

	return d
}

// Find operation: Go up the tree to the root of a component (set).
func (d *DSU) Find(x uint64) uint64 {
	// using path compression
	if d.parent[x] == x {
		return x
	}
	// moving pointers to the root of this set (union)
	d.parent[x] = d.Find(d.parent[x])
	return d.parent[x]
}

// Union smaller set to larger.
func (d *DSU) UnionSets(x, y int) {
	rootX := d.Find(uint64(x))
	rootY := d.Find(uint64(y))

	if rootX == rootY {
		return // already in the same set
	}

	// Smaller rank set to larger rank set
	if d.rank[rootX] > d.rank[rootY] {
		rootX, rootY = rootY, rootX
	}
	d.parent[rootX] = rootY
	if d.rank[rootX] == d.rank[rootY] {
		d.rank[rootY]++
	}
	d.setSize[rootY] += d.setSize[rootX]

	if d.maxElement[rootX] > d.maxElement[rootY] {
		d.maxElement[rootY] = d.maxElement[rootX]
	}
	if d.minElement[rootX] < d.minElement[rootY] {
		d.minElement[rootY] = d.minElement[rootX]
	}

	d.count--
}

func (d *DSU) IsConnected(x, y int) bool {
	return d.Find(uint64(x)) == d.Find(uint64(y))
}

// function returns a vector of all the elements from i to root of this union
func (d *DSU) GetParents(i uint64) []uint64 {
	ans := make([]uint64, 0)
	for d.parent[i] != i {
		ans = append(ans, i)
		i = d.parent[i]
	}
	ans = append(ans, i)
	return ans
}

func (d *DSU) Size(i uint64) uint64 {
	return d.setSize[d.Find(i)]
}

func (d *DSU) GetMax(i uint64) uint64 {
	return d.maxElement[d.Find(i)]
}

func (d *DSU) GetMin(i uint64) uint64 {
	return d.minElement[d.Find(i)]
}
