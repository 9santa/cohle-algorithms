package fenwick

// Operations: O(log n)
// Space: O(n)
type FenwickTreeMax struct {
	tree []int
}

func (f *FenwickTreeMax) lsb(i int) int {
	return i & -i
}

func NewFenwickTreeMax(size int) *FenwickTreeMax {
	return &FenwickTreeMax{tree: make([]int, size+1)}
}

// Construct Fenwick Tree.
func NewFenwickTreeMaxFromNums(nums []int) *FenwickTreeMax {
	ft := NewFenwickTreeMax(len(nums))
	for i, x := range nums {
		ft.Update(i, x)
	}
	return ft
}

func (f *FenwickTreeMax) Update(index, value int) {
	index++
	for index < len(f.tree) {
		f.tree[index] = max(f.tree[index], value)
		index += f.lsb(index)
	}
}

func (f *FenwickTreeMax) MaxQuery(index int) int {
	index++
	res := 0
	for index > 0 {
		res = max(res, f.tree[index])
		index -= f.lsb(index)
	}
	return res
}
