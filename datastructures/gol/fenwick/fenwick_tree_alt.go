package fenwick

// Operations: O(log n)
// Space: O(n)
type FenwickTreeAlt struct {
	tree []int
	nums []int
}

// Construct Fenwick Tree.
func NewFenwickTreeAlt(arr []int) *FenwickTreeAlt {
	ft := &FenwickTreeAlt{
		tree: make([]int, len(arr)),
		nums: append([]int(nil), arr...),
	}
	for i, x := range arr {
		ft.Update(i, x)
	}
	return ft
}

func (f *FenwickTreeAlt) Add(index, delta int) {
	j := index
	for j < len(f.tree) {
		f.tree[j] += delta
		j = j | (j + 1)
	}
}

// Update value at index.
func (f *FenwickTreeAlt) Update(index, value int) {
	diff := value - f.nums[index]
	f.nums[index] = value
	f.Add(index, diff)
}

// Get prefix sum [0, index].
func (f *FenwickTreeAlt) SumQuery(index int) int {
	j := index
	sum := 0
	for j >= 0 {
		sum += f.tree[j]
		j = (j & (j + 1)) - 1
	}
	return sum
}

// Get range sum [l, r] (same as prefix sum formula).
func (f *FenwickTreeAlt) RangeSumQuery(l, r int) int {
	if l == 0 {
		return f.SumQuery(r)
	}
	return f.SumQuery(r) - f.SumQuery(l-1)
}
