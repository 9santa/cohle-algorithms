package segtree

import "math/bits"

const PrimeSegMaxN = 100000

func binpowMod(a, b, mod int64) int64 {
	res := int64(1)
	for a %= mod; b > 0; b >>= 1 {
		if b&1 == 1 {
			res = (res * a) % mod
		}
		a = (a * a) % mod
	}
	return res
}

var (
	isPrimePrimeSeg [PrimeSegMaxN + 1]bool
	primesPrimeSeg  []int
	masksPrimeSeg   []uint64
	primeSegReady   bool
)

// O(N log log N)
func sievePrimeSeg() {
	for i := range isPrimePrimeSeg {
		isPrimePrimeSeg[i] = true
	}
	isPrimePrimeSeg[0] = false
	isPrimePrimeSeg[1] = false
	primesPrimeSeg = primesPrimeSeg[:0]
	for i := 2; i <= PrimeSegMaxN; i++ {
		if isPrimePrimeSeg[i] {
			primesPrimeSeg = append(primesPrimeSeg, i)
			if i <= PrimeSegMaxN/i {
				for j := i * i; j <= PrimeSegMaxN; j += i {
					isPrimePrimeSeg[j] = false
				}
			}
		}
	}
}

func computeMasksPrimeSeg() []uint64 {
	masks := make([]uint64, PrimeSegMaxN+1)
	for num := 1; num <= PrimeSegMaxN; num++ {
		var mask uint64
		limit := len(primesPrimeSeg)
		if limit > 64 {
			limit = 64
		}
		for i := 0; i < limit; i++ {
			if num%primesPrimeSeg[i] == 0 {
				mask |= uint64(1) << i
			}
		}
		masks[num] = mask
	}
	return masks
}

func ensurePrimeSegTables() {
	if primeSegReady {
		return
	}
	sievePrimeSeg()
	masksPrimeSeg = computeMasksPrimeSeg()
	primeSegReady = true
}

type PrimeSeg struct {
	n    int
	data []uint64
	lazy []uint64
}

func NewPrimeSeg(arr []int64) *PrimeSeg {
	ensurePrimeSegTables()
	size := 1
	for size < len(arr) {
		size <<= 1
	}
	p := &PrimeSeg{
		n:    size,
		data: make([]uint64, 2*size),
		lazy: make([]uint64, size),
	}
	p.build(arr)
	return p
}

func getPrimeMask(x int64) uint64 {
	if 0 <= x && x <= PrimeSegMaxN {
		return masksPrimeSeg[x]
	}
	return 0
}

func (p *PrimeSeg) apply(idx int, mask uint64) {
	p.data[idx] |= mask
	if idx < p.n {
		p.lazy[idx] |= mask
	}
}

// Push lazy down one level
func (p *PrimeSeg) push(idx int) {
	if p.lazy[idx] != 0 {
		p.apply(idx<<1, p.lazy[idx])
		p.apply(idx<<1|1, p.lazy[idx])
		p.lazy[idx] = 0
	}
}

// Push all lazy values on path from root to leaf, idx should be a leaf
func (p *PrimeSeg) pushPath(idx int) {
	for h := bits.Len(uint(p.n)) - 1; h > 0; h-- {
		i := idx >> h // ancestor at height h
		if p.lazy[i] != 0 {
			p.push(i)
		}
	}
}

// Build from array
func (p *PrimeSeg) build(arr []int64) {
	for i := 0; i < len(arr); i++ {
		p.data[i+p.n] = getPrimeMask(arr[i])
	}
	for i := p.n - 1; i > 0; i-- {
		p.data[i] = p.data[i<<1] | p.data[i<<1|1]
	}
}

// Range OR update: add prime divisors of x to range [l, r]
func (p *PrimeSeg) RangeOr(l, r int, x int64) {
	xmask := getPrimeMask(x)
	if xmask == 0 {
		return // no primes
	}
	l += p.n
	r += p.n
	l0, r0 := l, r
	p.pushPath(l0)
	p.pushPath(r0)

	// Apply OR updates
	for l <= r {
		if l&1 == 1 {
			p.apply(l, xmask)
			l++
		}
		if r&1 == 0 {
			p.apply(r, xmask)
			r--
		}
		l >>= 1
		r >>= 1
	}

	// Update parents
	for l = l0; l >= 1; l >>= 1 {
		if l < p.n && p.lazy[l] == 0 {
			p.data[l] = p.data[l<<1] | p.data[l<<1|1]
		}
	}
	for r = r0; r >= 1; r >>= 1 {
		if r < p.n && p.lazy[r] == 0 {
			p.data[r] = p.data[r<<1] | p.data[r<<1|1]
		}
	}
}

// Range query: get union of prime divisors in range [l, r]
func (p *PrimeSeg) RangeQuery(l, r int) uint64 {
	l += p.n
	r += p.n
	p.pushPath(l)
	p.pushPath(r)

	var res uint64
	for l <= r {
		if l&1 == 1 {
			res |= p.data[l]
			l++
		}
		if r&1 == 0 {
			res |= p.data[r]
			r--
		}
		l >>= 1
		r >>= 1
	}
	return res
}

// Point query: get prime mask at index idx
func (p *PrimeSeg) PointQuery(idx int) uint64 {
	idx += p.n
	p.pushPath(idx)
	return p.data[idx]
}

// Check if range [l, r] has all primes in mask
func (p *PrimeSeg) HasAll(l, r int, mask uint64) bool {
	rangeMask := p.RangeQuery(l, r)
	return rangeMask&mask == mask
}

// Check if range [l, r] has any prime in mask
func (p *PrimeSeg) HasAny(l, r int, mask uint64) bool {
	rangeMask := p.RangeQuery(l, r)
	return rangeMask&mask != 0
}

// Count distinct primes in range [l, r]
func (p *PrimeSeg) CountPrimes(l, r int) int {
	return bits.OnesCount64(p.RangeQuery(l, r))
}

// Get list of primes from mask
func (p *PrimeSeg) MaskToPrimes(mask uint64) []int {
	res := make([]int, 0)
	limit := len(primesPrimeSeg)
	if limit > 64 {
		limit = 64
	}
	for i := 0; i < limit; i++ {
		if mask&(uint64(1)<<i) != 0 {
			res = append(res, primesPrimeSeg[i])
		}
	}
	return res
}

// Get primes in range [l, r]
func (p *PrimeSeg) GetPrimesInRange(l, r int) []int {
	return p.MaskToPrimes(p.RangeQuery(l, r))
}
