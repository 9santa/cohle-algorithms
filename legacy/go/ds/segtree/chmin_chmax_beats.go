package segtree

import "math"

// Value-based version (Ji Driver style) - invariant: max1 and min1 always reflect real values
// Invariant chain: min1 < min2 <= max2 < max1
type Beat struct {
	Sum     int64
	Max1    int64
	Max2    int64
	Min1    int64
	Min2    int64 // max1 > max2, min1 < min2
	Maxc    int64
	Minc    int64
	LazyAdd int64
}

type BeatsSeg struct {
	n    int
	data []Beat
}

func NewBeatsSeg(arr []int64) *BeatsSeg {
	b := &BeatsSeg{
		n:    len(arr),
		data: make([]Beat, 4*max(1, len(arr))),
	}
	if len(arr) > 0 {
		b.build(arr, 1, 0, len(arr)-1)
	}
	return b
}

// Merge two kids into parent xd
func (b *BeatsSeg) pull(v int) {
	left := v << 1
	right := v<<1 | 1
	b.data[v].Sum = b.data[left].Sum + b.data[right].Sum

	// Max logic
	if b.data[left].Max1 == b.data[right].Max1 {
		b.data[v].Max1 = b.data[left].Max1
		b.data[v].Maxc = b.data[left].Maxc + b.data[right].Maxc
		b.data[v].Max2 = max(b.data[left].Max2, b.data[right].Max2)
	} else if b.data[left].Max1 > b.data[right].Max1 {
		b.data[v].Max1 = b.data[left].Max1
		b.data[v].Maxc = b.data[left].Maxc
		b.data[v].Max2 = max(b.data[left].Max2, b.data[right].Max1)
	} else {
		b.data[v].Max1 = b.data[right].Max1
		b.data[v].Maxc = b.data[right].Maxc
		b.data[v].Max2 = max(b.data[right].Max2, b.data[left].Max1)
	}

	// Min logic
	if b.data[left].Min1 == b.data[right].Min1 {
		b.data[v].Min1 = b.data[left].Min1
		b.data[v].Minc = b.data[left].Minc + b.data[right].Minc
		b.data[v].Min2 = min(b.data[left].Min2, b.data[right].Min2)
	} else if b.data[left].Min1 < b.data[right].Min1 {
		b.data[v].Min1 = b.data[left].Min1
		b.data[v].Minc = b.data[left].Minc
		b.data[v].Min2 = min(b.data[left].Min2, b.data[right].Min1)
	} else {
		b.data[v].Min1 = b.data[right].Min1
		b.data[v].Minc = b.data[right].Minc
		b.data[v].Min2 = min(b.data[left].Min1, b.data[right].Min2)
	}
}

// Apply generic addition
func (b *BeatsSeg) applyAdd(v, l, r int, val int64) {
	if val == 0 {
		return
	}
	b.data[v].Sum += int64(r-l+1) * val
	b.data[v].Max1 += val
	if b.data[v].Max2 != math.MinInt64 {
		b.data[v].Max2 += val
	}
	b.data[v].Min1 += val
	if b.data[v].Min2 != math.MaxInt64 {
		b.data[v].Min2 += val
	}
	b.data[v].LazyAdd += val
}

// Apply min cap (chmin)
func (b *BeatsSeg) applyMin(v int, val int64) {
	if b.data[v].Max1 <= val {
		return // break condition, nothing to do
	}
	b.data[v].Sum -= (b.data[v].Max1 - val) * b.data[v].Maxc
	b.data[v].Max1 = val
	if b.data[v].Min1 > val {
		b.data[v].Min1 = val // sync min1 if needed
	}
	if b.data[v].Min2 != math.MaxInt64 && b.data[v].Min2 > val { // sync min2 if needed
		b.data[v].Min2 = val
	}
}

// Apply max cap (chmax)
func (b *BeatsSeg) applyMax(v int, val int64) {
	if b.data[v].Min1 >= val {
		return // break condition, nothing to do
	}
	b.data[v].Sum += (val - b.data[v].Min1) * b.data[v].Minc
	b.data[v].Min1 = val
	if b.data[v].Max1 < val {
		b.data[v].Max1 = val // sync max1 if needed
	}
	if b.data[v].Max2 != math.MinInt64 && b.data[v].Max2 < val { // sync max2 if needed
		b.data[v].Max2 = val
	}
}

// Push lazy tags down one level
func (b *BeatsSeg) push(v, l, r int) {
	if l == r {
		return
	}
	mid := (l + r) >> 1
	L := v << 1
	R := v<<1 | 1
	// 1) Push add
	if b.data[v].LazyAdd != 0 {
		b.applyAdd(L, l, mid, b.data[v].LazyAdd)
		b.applyAdd(R, mid+1, r, b.data[v].LazyAdd)
		b.data[v].LazyAdd = 0
	}
	// 2) Push min (upper bound)
	// If parent's max is smaller than child's max, child must be constrained
	if b.data[L].Max1 > b.data[v].Max1 {
		b.applyMin(L, b.data[v].Max1)
	}
	if b.data[R].Max1 > b.data[v].Max1 {
		b.applyMin(R, b.data[v].Max1)
	}
	// 3) Push max (lower bound)
	// If parent's min is larger than child's min, child must be updated
	if b.data[L].Min1 < b.data[v].Min1 {
		b.applyMax(L, b.data[v].Min1)
	}
	if b.data[R].Min1 < b.data[v].Min1 {
		b.applyMax(R, b.data[v].Min1)
	}
}

func (b *BeatsSeg) build(arr []int64, v, l, r int) {
	b.data[v].LazyAdd = 0
	if l == r {
		b.data[v].Sum = arr[l]
		b.data[v].Max1 = arr[l]
		b.data[v].Min1 = arr[l]
		b.data[v].Max2 = math.MinInt64
		b.data[v].Min2 = math.MaxInt64
		b.data[v].Maxc = 1
		b.data[v].Minc = 1
		return
	}
	mid := (l + r) >> 1
	b.build(arr, v<<1, l, mid)
	b.build(arr, v<<1|1, mid+1, r)
	b.pull(v)
}

// a[i] = min(a[i], val)
func (b *BeatsSeg) updateChmin(v, tl, tr, l, r int, val int64) {
	if l > tr || r < tl || b.data[v].Max1 <= val {
		return // exit condition
	}
	if tl >= l && tr <= r && b.data[v].Max2 < val { // tag
		b.applyMin(v, val)
		return
	}
	b.push(v, tl, tr)
	tm := (tl + tr) >> 1
	b.updateChmin(v<<1, tl, tm, l, r, val)
	b.updateChmin(v<<1|1, tm+1, tr, l, r, val)
	b.pull(v)
}

// a[i] = max(a[i], val)
func (b *BeatsSeg) updateChmax(v, tl, tr, l, r int, val int64) {
	if l > tr || r < tl || b.data[v].Min1 >= val {
		return // exit condition
	}
	if tl >= l && tr <= r && b.data[v].Min2 > val { // tag
		b.applyMax(v, val)
		return
	}
	b.push(v, tl, tr)
	tm := (tl + tr) >> 1
	b.updateChmax(v<<1, tl, tm, l, r, val)
	b.updateChmax(v<<1|1, tm+1, tr, l, r, val)
	b.pull(v)
}

// a[i] = a[i] + val
func (b *BeatsSeg) updateAdd(v, tl, tr, l, r int, val int64) {
	if l > tr || r < tl {
		return
	}
	if tl >= l && tr <= r {
		b.applyAdd(v, tl, tr, val)
		return
	}
	b.push(v, tl, tr)
	tm := (tl + tr) >> 1
	b.updateAdd(v<<1, tl, tm, l, r, val)
	b.updateAdd(v<<1|1, tm+1, tr, l, r, val)
	b.pull(v)
}

func (b *BeatsSeg) querySum(v, tl, tr, l, r int) int64 {
	if l > tr || r < tl {
		return 0
	}
	if tl >= l && tr <= r {
		return b.data[v].Sum
	}
	b.push(v, tl, tr)
	tm := (tl + tr) >> 1
	return b.querySum(v<<1, tl, tm, l, r) +
		b.querySum(v<<1|1, tm+1, tr, l, r)
}

func (b *BeatsSeg) UpdateChmin(l, r int, val int64) {
	b.updateChmin(1, 0, b.n-1, l, r, val)
}

func (b *BeatsSeg) UpdateChmax(l, r int, val int64) {
	b.updateChmax(1, 0, b.n-1, l, r, val)
}

func (b *BeatsSeg) UpdateAdd(l, r int, val int64) {
	b.updateAdd(1, 0, b.n-1, l, r, val)
}

func (b *BeatsSeg) QuerySum(l, r int) int64 {
	return b.querySum(1, 0, b.n-1, l, r)
}
