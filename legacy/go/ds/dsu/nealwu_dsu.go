package dsu

// efficient & minimized DSU implementation stolen from Neal Wu
// don't know who came up with this, but super cute
type NealWuDSU struct {
	data  []int
	minEl []int
	maxEl []int
}

// usually want to pass n+1 to constructor
func NewNealWuDSU(n int) *NealWuDSU {
	d := &NealWuDSU{
		data:  make([]int, n),
		minEl: make([]int, n),
		maxEl: make([]int, n),
	}
	for i := 0; i < n; i++ {
		d.data[i] = -1
		d.minEl[i] = i
		d.maxEl[i] = i
	}
	return d
}

func (d *NealWuDSU) Find(x int) int {
	if d.data[x] < 0 {
		return x
	}
	d.data[x] = d.Find(d.data[x])
	return d.data[x]
}

func (d *NealWuDSU) UnionSets(x, y int) {
	x = d.Find(x)
	y = d.Find(y)
	if x == y {
		return
	}

	if d.data[x] > d.data[y] {
		x, y = y, x
	}

	d.data[x] += d.data[y]
	d.data[y] = x

	if d.minEl[y] < d.minEl[x] {
		d.minEl[x] = d.minEl[y]
	}
	if d.maxEl[y] > d.maxEl[x] {
		d.maxEl[x] = d.maxEl[y]
	}
}

func (d *NealWuDSU) SetSize(x int) int {
	return -d.data[d.Find(x)]
}
