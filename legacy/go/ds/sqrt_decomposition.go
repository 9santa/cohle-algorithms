package gol

import "math"

type SqrtDecomp struct {
	n      int
	len    int
	blocks int
	a      []int // copy of the input array
	b      []int // block sums
	mn     []int // block min
	mx     []int // block max
	zeros  []int // count of zeros in block
}

// preprocessing O(n)
func (s *SqrtDecomp) Build(arr []int) {
	s.a = append([]int(nil), arr...)
	s.n = len(s.a)
	s.len = int(math.Sqrt(float64(s.n))+1.0) + 1 // size of the block
	s.blocks = (s.n + s.len - 1) / s.len         // number of blocks
	s.b = make([]int, s.blocks)
	s.mn = make([]int, s.blocks)
	s.mx = make([]int, s.blocks)
	s.zeros = make([]int, s.blocks)
	for i := 0; i < s.blocks; i++ {
		s.mn[i] = int(^uint(0) >> 1)
		s.mx[i] = -s.mn[i] - 1
	}

	for i := 0; i < s.n; i++ {
		blk := i / s.len
		s.b[blk] += s.a[i]
		if s.mn[blk] > s.a[i] {
			s.mn[blk] = s.a[i]
		}
		if s.mx[blk] < s.a[i] {
			s.mx[blk] = s.a[i]
		}
		if s.a[i] == 0 {
			s.zeros[blk]++
		}
	}
}

// answering queries (bad) O(sqrt(n))
func (s *SqrtDecomp) BadSum(l, r int) int {
	res := 0
	for i := l; i <= r; {
		if i%s.len == 0 && i+s.len-1 <= r {
			res += s.b[i/s.len]
			i += s.len
		} else {
			res += s.a[i]
			i++
		}
	}
	return res
}

// answering queries (good) O(sqrt(n))
func (s *SqrtDecomp) Sum(l, r int) int {
	res := 0
	cl, cr := l/s.len, r/s.len
	if cl == cr {
		for i := l; i <= r; i++ {
			res += s.a[i]
		}
	} else {
		for i, end := l, (cl+1)*s.len-1; i <= end; i++ {
			res += s.a[i]
		}
		for i := cl + 1; i <= cr-1; i++ {
			res += s.b[i]
		}
		for i := cr * s.len; i <= r; i++ {
			res += s.a[i]
		}
	}
	return res
}

// point update
func (s *SqrtDecomp) Update(i, val int) {
	blk := i / s.len
	s.b[blk] += val - s.a[i]
	if s.a[i] == 0 {
		s.zeros[blk]--
	}
	if val == 0 {
		s.zeros[blk]++
	}

	maxValue := int(^uint(0) >> 1)
	minValue := -maxValue - 1
	s.mn[blk] = maxValue
	s.mx[blk] = minValue
	for j := blk * s.len; j < minInt(s.n, (blk+1)*s.len); j++ {
		if s.mn[blk] > s.a[j] {
			s.mn[blk] = s.a[j]
		}
		if s.mx[blk] < s.a[j] {
			s.mx[blk] = s.a[j]
		}
	}

	s.a[i] = val
}

type LazySqrt struct {
	n      int
	len    int
	blocks int
	a      []int64 // copy of the input array
	add    []int64 // lazy add per block
}

func (s *LazySqrt) Build(arr []int64) {
	s.a = append([]int64(nil), arr...)
	s.n = len(s.a)
	s.len = int(math.Sqrt(float64(s.n))) + 1
	s.blocks = (s.n + s.len - 1) / s.len
	s.add = make([]int64, s.blocks)
}

func (s *LazySqrt) RangeAdd(l, r int, delta int64) {
	bl := l / s.len
	br := r / s.len

	if bl == br {
		for i := l; i <= r; i++ {
			s.a[i] += delta
		}
		return
	}
	// left tail
	endLeft := (bl+1)*s.len - 1
	for i := l; i <= minInt(endLeft, s.n-1); i++ {
		s.a[i] += delta
	}
	// right tail
	startRight := br * s.len
	for i := startRight; i <= minInt(r, s.n-1); i++ {
		s.a[i] += delta
	}
	// full blocks between
	for b := bl + 1; b <= br-1; b++ {
		s.add[b] += delta
	}
}

func (s *LazySqrt) PointQuery(i int) int64 {
	b := i / s.len
	return s.a[i] + s.add[b]
}
