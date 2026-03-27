package rmq

/*
// --- RMQ +-1 ---
// Static RMQ in O(1) per query with Four Russians, O(sz(euler)) preprocessing
// Static RMQ -> Static LCA -> Static RMQ +-1 -> Four Russians -> O(1)
*/
type PlusMinusOneRMQ struct {
	n     int // length Euler depth array
	B     int // block size
	K     int // number of blocks
	types int // 2^(B-1)

	D           []int
	blockType   []int
	blockMinPos []int

	// micro[(t*B + i)*B + j] = argmin in [i..j] inside a block of type t
	micro []uint8

	// sparse table over blocks, storing global positions in D
	lg []int
	st [][]int
}

func NewPlusMinusOneRMQ(depths []int) *PlusMinusOneRMQ {
	r := &PlusMinusOneRMQ{}
	r.Build(depths)
	return r
}

func (r *PlusMinusOneRMQ) floorLog2(x int) int {
	res := 0
	for (1 << (res + 1)) <= x {
		res++
	}
	return res
}

func (r *PlusMinusOneRMQ) better(pos1, pos2 int) int {
	// compare by D value, tie by smaller index (leftmost)
	if r.D[pos1] != r.D[pos2] {
		if r.D[pos1] < r.D[pos2] {
			return pos1
		}
		return pos2
	}
	if pos1 < pos2 {
		return pos1
	}
	return pos2
}

func (r *PlusMinusOneRMQ) microAt(t, i, j int) uint8 {
	return r.micro[(t*r.B+i)*r.B+j]
}

func (r *PlusMinusOneRMQ) Build(depths []int) {
	r.D = append([]int(nil), depths...)
	r.n = len(r.D)
	if r.n == 0 {
		r.B = 1
		r.K = 0
		r.types = 1
		return
	}

	// choose B ~ 1/2 log2(n)
	logn := r.floorLog2(r.n)
	r.B = max(1, min(16, logn/2))
	r.types = 1 << max(0, r.B-1)

	r.K = (r.n + r.B - 1) / r.B
	r.blockType = make([]int, r.K)
	r.blockMinPos = make([]int, r.K)

	// precompute micro table for all types
	r.micro = make([]uint8, r.types*r.B*r.B)
	h := make([]int, r.B)
	for t := 0; t < r.types; t++ {
		h[0] = 0
		for k := 0; k < r.B-1; k++ {
			step := -1
			if (t>>k)&1 == 1 {
				step = +1
			}
			h[k+1] = h[k] + step
		}
		for i := 0; i < r.B; i++ {
			best := i
			bestVal := h[i]
			for j := i; j < r.B; j++ {
				if h[j] < bestVal {
					bestVal = h[j]
					best = j
				}
				r.micro[(t*r.B+i)*r.B+j] = uint8(best)
			}
		}
	}

	// compute block types and block mins
	for b := 0; b < r.K; b++ {
		start := b * r.B
		end := min(r.n-1, start+r.B-1)

		mask := 0
		for i := 0; i < r.B-1; i++ {
			pos := start + i
			if pos+1 >= r.n {
				break
			}
			if r.D[pos+1]-r.D[pos] == +1 {
				mask |= 1 << i
			}
		}
		r.blockType[b] = mask

		j := end - start
		off := r.microAt(mask, 0, j)
		r.blockMinPos[b] = start + int(off)
	}

	// build sparse table over block minima
	r.lg = make([]int, r.K+1)
	for i := 2; i <= r.K; i++ {
		r.lg[i] = r.lg[i/2] + 1
	}

	P := r.lg[r.K] + 1
	r.st = make([][]int, P)
	for p := range r.st {
		r.st[p] = make([]int, r.K)
	}
	for i := 0; i < r.K; i++ {
		r.st[0][i] = r.blockMinPos[i]
	}

	for p := 1; p < P; p++ {
		length := 1 << p
		half := length >> 1
		for i := 0; i+length <= r.K; i++ {
			r.st[p][i] = r.better(r.st[p-1][i], r.st[p-1][i+half])
		}
	}
}

func (r *PlusMinusOneRMQ) QueryBlocks(L, R int) int {
	// inclusive block indices, requires L <= R
	if L > R {
		panic("QueryBlocks: invalid block range")
	}
	length := R - L + 1
	p := r.lg[length]
	a := r.st[p][L]
	b := r.st[p][R-(1<<p)+1]
	return r.better(a, b)
}

func (r *PlusMinusOneRMQ) Query(l, rr int) int {
	// inclusive positions in D
	if l > rr {
		l, rr = rr, l
	}
	bl := l / r.B
	br := rr / r.B
	if bl == br {
		i := l % r.B
		j := rr % r.B
		off := r.microAt(r.blockType[bl], i, j)
		return bl*r.B + int(off)
	}

	// left tail: [l .. end of left block]
	leftPos := bl*r.B + int(r.microAt(r.blockType[bl], l%r.B, r.B-1))
	// right tail: [start of right block .. r]
	rightPos := br*r.B + int(r.microAt(r.blockType[br], 0, rr%r.B))

	bestPos := r.better(leftPos, rightPos)

	// middle full blocks
	if bl+1 <= br-1 {
		midPos := r.QueryBlocks(bl+1, br-1)
		bestPos = r.better(bestPos, midPos)
	}
	return bestPos
}
