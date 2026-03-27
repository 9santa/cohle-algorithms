package gol

// C++14-compatible variant in the original tree.
// In Go this is the same reusable free-list pool as node_pool.go, because there is no aligned_storage split.
type NodePoolC14[T any] struct {
	pool *NodePool[T]
}

func NewNodePoolC14[T any]() *NodePoolC14[T] {
	return &NodePoolC14[T]{pool: NewNodePool[T]()}
}

func (p *NodePoolC14[T]) ResetKeepMemory() { p.pool.ResetKeepMemory() }
func (p *NodePoolC14[T]) Release()         { p.pool.Release() }
func (p *NodePoolC14[T]) ReserveNodes(n int) {
	p.pool.ReserveNodes(n)
}
func (p *NodePoolC14[T]) Create(init func(*T)) *T { return p.pool.Create(init) }
func (p *NodePoolC14[T]) Clone(x *T) *T           { return p.pool.Clone(x) }
func (p *NodePoolC14[T]) Destroy(x *T)            { p.pool.Destroy(x) }
