package gol

/*
Arena allocator + free-list
Instead of: new Node() -> heap allocation per node, delete node -> heap free per node,
we keep a free-list of reusable objects and let Go's runtime manage the backing storage.
This preserves the original reuse pattern without translating placement-new/aligned raw storage directly.
*/

// 2^12 = 4096 slots per chunk
const defaultNodePoolChunkBits = 12

type NodePool[T any] struct {
	free []*T
	all  []*T
}

func NewNodePool[T any]() *NodePool[T] {
	p := &NodePool[T]{}
	p.addChunk(1 << defaultNodePoolChunkBits)
	return p
}

// O(1): reuses already-allocated nodes; does NOT zero values eagerly
func (p *NodePool[T]) ResetKeepMemory() {
	p.free = p.free[:0]
	p.free = append(p.free, p.all...)
}

// Frees all references and allocates a fresh first chunk
func (p *NodePool[T]) Release() {
	p.free = nil
	p.all = nil
	p.addChunk(1 << defaultNodePoolChunkBits)
}

// Ensure capacity for at least n nodes
func (p *NodePool[T]) ReserveNodes(n int) {
	if cap(p.all) >= n {
		return
	}
	p.addChunk(n - len(p.all))
}

func (p *NodePool[T]) Create(init func(*T)) *T {
	x := p.acquireSlot()
	var zero T
	*x = zero
	if init != nil {
		init(x)
	}
	return x
}

func (p *NodePool[T]) Clone(x *T) *T {
	if x == nil {
		return nil
	}
	y := p.acquireSlot()
	*y = *x
	return y
}

func (p *NodePool[T]) Destroy(x *T) {
	if x == nil {
		return
	}
	p.free = append(p.free, x)
}

func (p *NodePool[T]) addChunk(size int) {
	if size <= 0 {
		return
	}
	chunk := make([]T, size)
	for i := range chunk {
		p.all = append(p.all, &chunk[i])
		p.free = append(p.free, &chunk[i])
	}
}

func (p *NodePool[T]) acquireSlot() *T {
	if len(p.free) == 0 {
		p.addChunk(maxInt(1, len(p.all)))
	}
	x := p.free[len(p.free)-1]
	p.free = p.free[:len(p.free)-1]
	return x
}
