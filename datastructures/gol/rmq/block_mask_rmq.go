package rmq

import (
	"math/bits"

	"cohlealgorithms/datastructures/gol/sparsetable"
)

/*
   RMQ for any array D[] using "block + mask-in-block + sparse table over blocks"
   D[] will be Euler depths (adj diffs = +-1), but mask trick work any values
   Block size B is chosen ~log2(n), must be <= 63 (we use uint64 masks)
   In-block small range RMQ(l, r) is answered in O(1) via:
       m = mask[r] & ((1ULL << len) - 1), pos = msb(m), ans = r - pos
   // Can be used with: farach-colton-bender algo
*/

// RMQ-TYPE MONOIDS ONLY (min, max, argmin, argmax...)
type BlockMaskRMQ[T any] struct {
	n int
	B int // block length
	K int // block count

	a    []T
	mask []uint64 // mask[r] for each position r

	blockBestPos []int // for each block, global index of best
	blockBestVal []T   // blockBestVal[b] = a[blockBestPos[b]]

	st     *sparsetable.SparseTableIdempotent[T] // sparse table over blockBestVal
	better func(T, T) bool
	id     func() T
}

func NewBlockMaskRMQ[T any](arr []T, better func(T, T) bool, id func() T) *BlockMaskRMQ[T] {
	r := &BlockMaskRMQ[T]{better: better, id: id}
	r.Build(arr)
	return r
}

// choose better index among i, j using better()
func (r *BlockMaskRMQ[T]) betterIdx(i, j int) int {
	if r.better(r.a[i], r.a[j]) {
		return i
	}
	return j
}

// in-block query, assumes l and r in same block
func (r *BlockMaskRMQ[T]) QueryInBlock(l, rr int) int {
	length := rr - l + 1
	var bitsMask uint64
	if length >= 64 {
		bitsMask = ^uint64(0)
	} else {
		bitsMask = (uint64(1) << length) - 1
	}
	m := r.mask[rr] & bitsMask
	pos := bits.Len64(m) - 1
	return rr - pos
}

// query best among full blocks [L..R] inclusive block indices, returns global index
func (r *BlockMaskRMQ[T]) QueryBlocks(L, R int) int {
	bi := r.st.QueryIndex(L, R) // index in blockBestVal
	return r.blockBestPos[bi]   // global index in a
}

func (r *BlockMaskRMQ[T]) Build(arr []T) {
	r.a = append([]T(nil), arr...)
	r.n = len(r.a)
	if r.n == 0 {
		r.B = 1
		r.K = 0
		r.mask = nil
		r.blockBestPos = nil
		r.blockBestVal = nil
		r.st = nil
		return
	}

	// Choose max block size ~log2(n), clamp to [1..63]
	r.B = max(1, min(63, bits.Len(uint(r.n))-1))
	r.K = (r.n + r.B - 1) / r.B
	r.mask = make([]uint64, r.n)
	r.blockBestPos = make([]int, r.K)
	r.blockBestVal = make([]T, r.K)

	// Build masks per block using a monotone (Cartesian) stack
	// pop while new is strictly better than top (better(new, top) == true)
	// tie behavior is controlled by better
	for b := 0; b < r.K; b++ {
		L := b * r.B
		R := min(r.n-1, L+r.B-1)
		var curMask uint64
		keep := uint64(1<<r.B) - 1
		if r.B == 64 {
			keep = ^uint64(0)
		}
		stk := make([]int, 0, r.B)

		for i := L; i <= R; i++ {
			curMask = ((curMask << 1) | 1) & keep
			for len(stk) > 0 && r.better(r.a[i], r.a[stk[len(stk)-1]]) {
				j := stk[len(stk)-1]
				stk = stk[:len(stk)-1]
				off := i - j // 1..B-1
				curMask &^= uint64(1) << off
			}
			r.mask[i] = curMask
			stk = append(stk, i)
		}

		// best of whole block
		best := r.QueryInBlock(L, R)
		r.blockBestPos[b] = best
		r.blockBestVal[b] = r.a[best]
	}

	// sparse table over blocks
	r.st = sparsetable.NewSparseTableIdempotent(r.blockBestVal, r.better, r.id)
}

// RMQ on D[l..r], returns global index of best
func (r *BlockMaskRMQ[T]) QueryIndex(l, rr int) int {
	if l > rr {
		l, rr = rr, l
	}
	bl := l / r.B
	br := rr / r.B
	if bl == br {
		return r.QueryInBlock(l, rr)
	}

	leftEnd := min(r.n-1, bl*r.B+r.B-1)
	rightStart := br * r.B

	ans := r.betterIdx(r.QueryInBlock(l, leftEnd), r.QueryInBlock(rightStart, rr))
	if bl+1 <= br-1 {
		ans = r.betterIdx(ans, r.QueryBlocks(bl+1, br-1))
	}
	return ans
}

func (r *BlockMaskRMQ[T]) QueryValue(l, rr int) T {
	return r.a[r.QueryIndex(l, rr)]
}

func min(a, b int) int {
	if a > b {
		return b
	}
	return a
}

func max(a, b int) int {
	if a < b {
		return b
	}
	return a
}
