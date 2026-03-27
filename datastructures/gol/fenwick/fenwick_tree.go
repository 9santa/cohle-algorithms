package fenwick

import "math/bits"

// Operations: O(log n)
// Space: O(n)
type FenwickTree struct {
	tree []int
}

func (f *FenwickTree) lsb(i int) int {
	return i & -i
}

func NewFenwickTree(size int) *FenwickTree {
	return &FenwickTree{tree: make([]int, size+1)}
}

// Construct Fenwick Tree.
func NewFenwickTreeFromNums(nums []int) *FenwickTree {
	ft := NewFenwickTree(len(nums))
	for i, x := range nums {
		ft.Update(i, x)
	}
	return ft
}

// O(n) Fenwick initialization.
func (f *FenwickTree) Init(n int, nums []int) {
	f.tree = make([]int, n+1)
	for i := 1; i <= n; i++ {
		f.tree[i] += nums[i]
		if i+f.lsb(i) <= n {
			f.tree[i+f.lsb(i)] += f.tree[i]
		}
	}
}

func (f *FenwickTree) Update(index, value int) {
	index++
	for index < len(f.tree) {
		f.tree[index] += value
		index += f.lsb(index)
	}
}

// Get prefix sum [0, index].
func (f *FenwickTree) SumQuery(index int) int {
	index++
	sum := 0
	for index > 0 {
		sum += f.tree[index]
		index -= f.lsb(index)
	}
	return sum
}

// Get range sum [l, r] (same as prefix sum formula).
func (f *FenwickTree) RangeSumQuery(l, r int) int {
	return f.SumQuery(r) - f.SumQuery(l-1)
}

// 0-based index of first prefix sum >= x.
// prefix sums have to be monotone for this.
func (f *FenwickTree) LowerBound(x int) int {
	pos := 0
	sum := 0
	for i := bits.Len(uint(len(f.tree))) - 1; i >= 0; i-- {
		nxt := pos + (1 << i)
		if nxt < len(f.tree) && sum+f.tree[nxt] < x {
			sum += f.tree[nxt]
			pos = nxt
		}
	}
	return pos
}
