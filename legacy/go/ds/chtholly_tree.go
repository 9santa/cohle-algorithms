package gol

import "sort"

type OrderedSigned interface {
	~int | ~int8 | ~int16 | ~int32 | ~int64
}

type Pair[T any] struct {
	First  T
	Second int64
}

/*
Lazy, interval-based ds that uses map

	to store contiguous intervals with the same value
	Efficient for certain types of randomized array ops
*/
type Chtholly[T OrderedSigned] struct {
	mp map[int]T // stores intervals [l, r) with value
	n  int
}

func binpow(a, b, mod int) int {
	res := 1 % mod
	for b > 0 {
		if b&1 == 1 {
			res = int(int64(res) * int64(a) % int64(mod))
		}
		a = int(int64(a) * int64(a) % int64(mod))
		b >>= 1
	}
	return res
}

// Init from array.
func NewChthollyFromArray[T OrderedSigned](arr []T) *Chtholly[T] {
	ch := &Chtholly[T]{mp: make(map[int]T), n: len(arr)}
	for i := 0; i < ch.n; {
		j := i
		for j < ch.n && arr[j] == arr[i] {
			j++
		}
		ch.mp[i] = arr[i]
		i = j
	}
	var zero T
	ch.mp[ch.n] = zero // sentinel
	return ch
}

// Init with all 'initial'.
func NewChtholly[T OrderedSigned](size int, initial T) *Chtholly[T] {
	ch := &Chtholly[T]{mp: make(map[int]T), n: size}
	ch.mp[0] = initial
	var minusOne T = -1
	ch.mp[ch.n] = minusOne
	return ch
}

func (c *Chtholly[T]) keys() []int {
	keys := make([]int, 0, len(c.mp))
	for k := range c.mp {
		keys = append(keys, k)
	}
	sort.Ints(keys)
	return keys
}

func (c *Chtholly[T]) split(i int) {
	if i >= c.n {
		return
	}
	keys := c.keys()
	prevKey := -1
	for _, k := range keys {
		if k > i {
			break
		}
		prevKey = k
	}
	if prevKey != -1 && prevKey != i {
		c.mp[i] = c.mp[prevKey]
	}
}

// Range add: add x to [l, r).
func (c *Chtholly[T]) RangeAdd(l, r int, x T) {
	if l >= r {
		return
	}
	c.split(l)
	c.split(r)
	for _, k := range c.keys() {
		if k < l {
			continue
		}
		if k == r {
			break
		}
		c.mp[k] += x
	}
}

func (c *Chtholly[T]) RangeAssign(l, r int, x T) {
	if l >= r {
		return
	}
	c.split(l)
	c.split(r)
	for _, k := range c.keys() {
		if k < l || k == r {
			continue
		}
		if k > r {
			break
		}
		delete(c.mp, k)
	}
	c.mp[l] = x
}

// Get k-th smallest element in [l, r) (0-idx).
func (c *Chtholly[T]) RangeKth(l, r, k int) (T, bool) {
	var zero T
	if l >= r || k < 0 {
		return zero, false
	}
	c.split(l)
	c.split(r)

	segments := make([]Pair[T], 0)
	keys := c.keys()
	for idx, key := range keys {
		if key < l {
			continue
		}
		if key == r {
			break
		}
		if idx+1 >= len(keys) {
			break
		}
		segments = append(segments, Pair[T]{First: c.mp[key], Second: int64(keys[idx+1] - key)})
	}

	sort.Slice(segments, func(i, j int) bool {
		if segments[i].First == segments[j].First {
			return segments[i].Second < segments[j].Second
		}
		return segments[i].First < segments[j].First
	})

	for _, seg := range segments {
		if int64(k) < seg.Second {
			return seg.First, true
		}
		k -= int(seg.Second)
	}

	return zero, false // k out of range
}

// Get sum of (a[i]^x mod M) over [l, r).
func (c *Chtholly[T]) RangePowerSum(l, r, x, mod int) int64 {
	if l >= r {
		return 0
	}
	c.split(l)
	c.split(r)

	var ans int64
	keys := c.keys()
	for idx, key := range keys {
		if key < l {
			continue
		}
		if key == r {
			break
		}
		if idx+1 >= len(keys) {
			break
		}
		length := int64(keys[idx+1] - key)
		val := int(c.mp[key]) % mod
		ans = (ans + length*int64(binpow(val, x, mod))) % int64(mod)
	}
	return ans
}

// Get all values in [l, r) as vector of pairs {value, length}.
func (c *Chtholly[T]) GetRange(l, r int) []Pair[T] {
	if l >= r {
		return nil
	}
	c.split(l)
	c.split(r)

	res := make([]Pair[T], 0)
	keys := c.keys()
	for idx, key := range keys {
		if key < l {
			continue
		}
		if key == r {
			break
		}
		if idx+1 >= len(keys) {
			break
		}
		res = append(res, Pair[T]{First: c.mp[key], Second: int64(keys[idx+1] - key)})
	}
	return res
}

// Get value at position i.
func (c *Chtholly[T]) Get(i int) (T, bool) {
	var zero T
	if i < 0 || i >= c.n {
		return zero, false
	}
	prevKey := -1
	for _, k := range c.keys() {
		if k > i {
			break
		}
		prevKey = k
	}
	if prevKey == -1 {
		return zero, false
	}
	return c.mp[prevKey], true
}
