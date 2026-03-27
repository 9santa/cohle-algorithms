package segtree

// Verification: https://codeforces.com/contest/242/problem/E

// Data is an array of bit counts.
type SumXorBits struct {
	Bits []int64
}

func newSumXorBits(B int) SumXorBits {
	return SumXorBits{Bits: make([]int64, B)}
}

type SumXorLazySegtree struct {
	B   int
	seg *LazySegtree[SumXorBits, int]
}

func NewSumXorLazySegtree(arr []int64, B int) *SumXorLazySegtree {
	st := &SumXorLazySegtree{B: B}
	st.seg = NewLazySegtreeFromSlice(
		liftAllBits(arr, B),
		func(x, y SumXorBits) SumXorBits {
			res := newSumXorBits(B)
			for i := 0; i < B; i++ {
				res.Bits[i] = x.Bits[i] + y.Bits[i]
			}
			return res
		},
		func() SumXorBits { return newSumXorBits(B) },
		func(x, y int) int { return x ^ y },
		func() int { return 0 },
		func(a, b int) bool { return a == b },
		func(x SumXorBits, a int, size int64) SumXorBits {
			if a == 0 {
				return x
			}
			res := newSumXorBits(B)
			copy(res.Bits, x.Bits)
			for i := 0; i < B; i++ {
				if (a>>i)&1 == 1 {
					// if bit i is flipped, new count of 1s is (total size - old count)
					res.Bits[i] = size - res.Bits[i]
				}
			}
			return res
		},
	)
	return st
}

// Convert input number to bit-count array.
func liftBits(val int64, B int) SumXorBits {
	res := newSumXorBits(B)
	for i := 0; i < B; i++ {
		res.Bits[i] = (val >> i) & 1
	}
	return res
}

func liftAllBits(arr []int64, B int) []SumXorBits {
	res := make([]SumXorBits, len(arr))
	for i, x := range arr {
		res[i] = liftBits(x, B)
	}
	return res
}

// Convert bit-count array back to num.
func SumBits(x SumXorBits) int64 {
	var s int64
	for i := 0; i < len(x.Bits); i++ {
		s += x.Bits[i] * (1 << i)
	}
	return s
}

func (s *SumXorLazySegtree) Set(p int, x int64) {
	s.seg.Set(p, liftBits(x, s.B))
}

func (s *SumXorLazySegtree) Get(p int) SumXorBits {
	return s.seg.Get(p)
}

func (s *SumXorLazySegtree) GetAll() []SumXorBits {
	return s.seg.GetAll()
}

// [l, r)
func (s *SumXorLazySegtree) Prod(l, r int) SumXorBits {
	return s.seg.Prod(l, r)
}

func (s *SumXorLazySegtree) ProdAll() SumXorBits {
	return s.seg.ProdAll()
}

func (s *SumXorLazySegtree) Apply(l, r, mask int) {
	s.seg.Apply(l, r, mask)
}
