package sparsetable

import (
	"math"
	"math/bits"
)

// Sparse Table for Range Minimum Queries
// For Maximum Queries: just change math.MaxInt to math.MinInt, and min to max
// Prep: O(n log n), Queries: O(1)
type SparseTableRMQ struct {
	table   [][]int
	n       int // original array size
	paddedN int // padded size (closest power of 2)
}

// calculate next power of 2 >= x
func nextPowerOf2(x int) int {
	if x <= 0 {
		return 1
	}
	if x == 1 {
		return 2
	}
	return 1 << bits.Len(uint(x-1))
}

func NewSparseTableRMQ(arr []int) *SparseTableRMQ {
	st := &SparseTableRMQ{n: len(arr)}
	if st.n == 0 {
		return st
	}

	// pad to closest power of 2
	st.paddedN = nextPowerOf2(st.n)

	kMax := floorLog2(st.paddedN) + 1
	st.table = make([][]int, kMax)
	for i := range st.table {
		st.table[i] = make([]int, st.paddedN)
		for j := range st.table[i] {
			st.table[i][j] = math.MaxInt
		}
	}

	// Level 0: intervals of length 1 (2^0)
	for i, x := range arr {
		st.table[0][i] = x
	}

	// Build table for intervals of length 2^j
	for j := 1; j < kMax; j++ {
		step := 1 << (j - 1) // half the current interval length
		for i := 0; i+(1<<j) <= st.n; i++ {
			st.table[j][i] = min(st.table[j-1][i], st.table[j-1][i+step])
		}
	}

	return st
}

// Query for minimum in range [l, r] (0-indexed, inclusive)
func (st *SparseTableRMQ) Query(l, r int) int {
	if st.n == 0 {
		return math.MaxInt
	}
	if l < 0 {
		l = 0
	}
	if r >= st.n {
		r = st.n - 1
	}
	if l > r {
		return math.MaxInt
	}

	length := r - l + 1
	k := floorLog2(length)
	return min(st.table[k][l], st.table[k][r-(1<<k)+1])
}
