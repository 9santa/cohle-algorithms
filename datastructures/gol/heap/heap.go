package heap

// decent human-like heap implementation (binary min/max heap)
// each node is min/max in its subtree
type Heap struct {
	h []int
}

func NewHeap(capacity int) *Heap {
	return &Heap{h: make([]int, 0, capacity)}
}

// O(1)
func (h *Heap) GetMin() (int, bool) {
	if len(h.h) == 0 {
		return 0, false
	}
	return h.h[0], true
}

// O(log n)
func (h *Heap) Insert(x int) {
	h.h = append(h.h, x)
	// sift-up
	i := len(h.h) - 1
	for i > 0 && h.h[i] < h.h[(i-1)/2] {
		h.h[i], h.h[(i-1)/2] = h.h[(i-1)/2], h.h[i]
		i = (i - 1) / 2
	}
}

// O(log n)
func (h *Heap) RemoveMin() bool {
	if len(h.h) == 0 {
		return false
	}

	// sift-down
	last := len(h.h) - 1
	h.h[0], h.h[last] = h.h[last], h.h[0]
	h.h = h.h[:last]

	i := 0
	for 2*i+1 < len(h.h) {
		j := 2*i + 1
		if 2*i+2 < len(h.h) && h.h[2*i+2] < h.h[j] {
			j = 2*i + 2
		}
		if h.h[i] <= h.h[j] {
			break
		}
		h.h[i], h.h[j] = h.h[j], h.h[i]
		i = j
	}
	return true
}
