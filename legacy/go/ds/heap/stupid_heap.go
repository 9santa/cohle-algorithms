package heap

import "math"

// The simplest & slowest & dumbest heap implementation
type HeapV1 struct {
	h []int
}

// insert an elemenet at the end. O(1)
func (h *HeapV1) Insert(x int) {
	h.h = append(h.h, x)
}

// get minimum element in the heap. O(n)
func (h *HeapV1) GetMin() (int, bool) {
	if len(h.h) == 0 {
		return 0, false
	}
	res := math.MaxInt
	for _, x := range h.h {
		if x < res {
			res = x
		}
	}
	return res, true
}

// remove min of the heap. O(n)
func (h *HeapV1) RemoveMin() bool {
	if len(h.h) == 0 {
		return false
	}
	j := 0
	for i := 1; i < len(h.h); i++ {
		if h.h[j] > h.h[i] {
			j = i
		}
	}
	last := len(h.h) - 1
	h.h[j], h.h[last] = h.h[last], h.h[j]
	h.h = h.h[:last]
	return true
}

// heap with storing the min element
type HeapV2 struct {
	h []int
	m int
}

// O(1)
func (h *HeapV2) GetMin() (int, bool) {
	if len(h.h) == 0 {
		return 0, false
	}
	return h.h[h.m], true
}

// O(n)
func (h *HeapV2) RemoveMin() bool {
	if len(h.h) == 0 {
		return false
	}
	last := len(h.h) - 1
	h.h[h.m], h.h[last] = h.h[last], h.h[h.m]
	h.h = h.h[:last]

	// update new minimum
	h.m = 0
	for i := 1; i < len(h.h); i++ {
		if h.h[i] < h.h[h.m] {
			h.m = i
		}
	}
	return true
}

// O(1)
func (h *HeapV2) Insert(x int) {
	h.h = append(h.h, x)
	if len(h.h) == 1 || x < h.h[h.m] {
		h.m = len(h.h) - 1
	}
}

// heap with fast minimum element deletion
type HeapV3 struct {
	h []int
}

// O(1)
func (h *HeapV3) RemoveMin() bool {
	if len(h.h) == 0 {
		return false
	}
	h.h = h.h[:len(h.h)-1] // careful no out of bounds check here in the original
	return true
}

// O(1)
func (h *HeapV3) GetMin() (int, bool) {
	if len(h.h) == 0 {
		return 0, false
	}
	return h.h[len(h.h)-1], true
}

// sift-left (1-run of insertion sort). O(n)
func (h *HeapV3) Insert(x int) {
	h.h = append(h.h, x)
	j := len(h.h) - 1
	for j > 0 && h.h[j] > h.h[j-1] {
		h.h[j], h.h[j-1] = h.h[j-1], h.h[j]
		j--
	}
}
