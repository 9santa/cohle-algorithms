package gol

import (
	"container/heap"
	"math"
)

type Line struct {
	A float64 // f(t) = A*t + B
	B float64
}

func (l Line) Eval(t float64) float64 { return l.A*t + l.B }

type Event struct {
	Time    float64
	Node    int
	Version int
}

type eventHeap []Event

func (h eventHeap) Len() int { return len(h) }
func (h eventHeap) Less(i, j int) bool {
	if h[i].Time != h[j].Time {
		return h[i].Time < h[j].Time
	}
	return h[i].Node < h[j].Node
}
func (h eventHeap) Swap(i, j int) { h[i], h[j] = h[j], h[i] }
func (h *eventHeap) Push(x any)   { *h = append(*h, x.(Event)) }
func (h *eventHeap) Pop() any {
	old := *h
	n := len(old)
	x := old[n-1]
	*h = old[:n-1]
	return x
}

type KineticTournamentMax struct {
	n        int
	base     int       // first leaf index in segment-tree layout
	now      float64   // current processed time (monotone non-decreasing)
	f        []Line    // trajectories (size n)
	winner   []int     // winner[node] = index of winning line (or -1)
	failTime []float64 // fail_time[node] = certificate failure time
	ver      []int     // version counter per node

	pq eventHeap
}

const kineticEPS = 1e-12

func NewKineticTournamentMax(lines []Line) *KineticTournamentMax {
	k := &KineticTournamentMax{n: len(lines), f: append([]Line(nil), lines...)}
	k.build()
	return k
}

func (k *KineticTournamentMax) build() {
	k.base = 1
	for k.base < k.n {
		k.base <<= 1
	}
	N := 2 * k.base
	k.winner = make([]int, N)
	k.failTime = make([]float64, N)
	k.ver = make([]int, N)
	for i := 0; i < N; i++ {
		k.winner[i] = -1
		k.failTime[i] = math.Inf(1)
	}
	// init leaves
	for i := 0; i < k.n; i++ {
		k.winner[k.base+i] = i
	}
	heap.Init(&k.pq)
	// build internal nodes bottom-up
	for i := k.base - 1; i >= 1; i-- {
		k.pull(i)
	}
}

func (k *KineticTournamentMax) better(a, b int, t float64) bool {
	// true if a >= b at time t, under tie-break by smaller index
	if a < 0 {
		return false
	}
	if b < 0 {
		return true
	}
	va := k.f[a].Eval(t)
	vb := k.f[b].Eval(t)
	if math.Abs(va-vb) > 1e-9 { // EPS for double precision mistakes
		return va > vb
	}
	return a < b
}

func (k *KineticTournamentMax) nextFailureTime(w, l int, tnow float64) float64 {
	// w is the current winner, l is the current loser; compute earliest t > tnow when l overtakes w
	if w < 0 || l < 0 {
		return math.Inf(1)
	}
	aw, bw := k.f[w].A, k.f[w].B
	al, bl := k.f[l].A, k.f[l].B

	// If slopes are equal, overtake never happens
	if math.Abs(aw-al) < 1e-15 {
		return math.Inf(1)
	}

	// Crossing time: solve aw*t + bw = al*t + bl
	tCross := (bl - bw) / (aw - al)
	if !(tCross > tnow+kineticEPS) { // must be in the future
		return math.Inf(1)
	}

	// For linear functions: after crossing, the larger slope wins
	if al > aw {
		return tCross
	}
	return math.Inf(1)
}

func (k *KineticTournamentMax) schedule(v int, tFail float64) {
	k.failTime[v] = tFail
	k.ver[v]++
	if tFail < math.Inf(1) {
		heap.Push(&k.pq, Event{Time: tFail, Node: v, Version: k.ver[v]})
	}
}

func (k *KineticTournamentMax) pull(v int) {
	L, R := v<<1, v<<1|1
	wl := k.winner[L]
	wr := k.winner[R]

	if wl < 0 && wr < 0 {
		k.winner[v] = -1
		k.schedule(v, math.Inf(1))
		return
	}

	w := wr
	if k.better(wl, wr, k.now) {
		w = wl
	}
	l := wl
	if w == wl {
		l = wr
	}
	k.winner[v] = w

	// certificate: w >= l until fail time
	tFail := k.nextFailureTime(w, l, k.now)
	k.schedule(v, tFail)
}

func (k *KineticTournamentMax) advanceTo(tNew float64) {
	if tNew < k.now {
		return
	}

	for k.pq.Len() > 0 {
		e := k.pq[0]
		if e.Time > tNew {
			break
		}
		heap.Pop(&k.pq)

		if e.Node <= 0 || e.Node >= len(k.failTime) {
			continue
		}
		if e.Version != k.ver[e.Node] {
			continue
		}
		if math.Abs(e.Time-k.failTime[e.Node]) > 1e-9 {
			continue
		}

		// Move time to just after the event (when certificate just failed)
		k.now = math.Nextafter(e.Time, math.Inf(1))

		// Recompute this node and all its ancestors, because winners may change above as well
		for v := e.Node; v >= 1; v >>= 1 {
			k.pull(v)
			if v == 1 {
				break
			}
		}
	}

	// Finally set 'now' to tNew
	k.now = tNew
}

// Return index of maximum at time t (0-indexed)
func (k *KineticTournamentMax) Argmax(t float64) int {
	k.advanceTo(t)
	return k.winner[1]
}

// Return maximum value at time t
func (k *KineticTournamentMax) MaxValue(t float64) float64 {
	idx := k.Argmax(t)
	if idx < 0 {
		return math.Inf(-1)
	}
	return k.f[idx].Eval(t)
}

// Update one trajectory at current processed time
func (k *KineticTournamentMax) SetLine(idx int, line Line) {
	if idx < 0 || idx >= k.n {
		return
	}
	k.f[idx] = line
	leaf := k.base + idx
	// Recompute along the path to root at current time 'now'
	for v := leaf >> 1; v >= 1; v >>= 1 {
		k.pull(v)
		if v == 1 {
			break
		}
	}
}
