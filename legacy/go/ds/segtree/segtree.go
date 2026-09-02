package segtree

type SegTree[T any] struct {
	dat  []T
	n    int
	log  int
	size int
	op   func(T, T) T
	id   func() T
}

func NewSegTree[T any](n int, op func(T, T) T, id func() T) *SegTree[T] {
	st := &SegTree[T]{op: op, id: id}
	st.Build(n, func(int) T { return id() })
	return st
}

func NewSegTreeFromSlice[T any](v []T, op func(T, T) T, id func() T) *SegTree[T] {
	st := &SegTree[T]{op: op, id: id}
	st.Build(len(v), func(i int) T { return v[i] })
	return st
}

func (st *SegTree[T]) Build(m int, f func(int) T) {
	st.n = m
	st.log = 1
	for (1 << st.log) < st.n {
		st.log++
	}
	st.size = 1 << st.log
	st.dat = make([]T, st.size<<1)
	for i := range st.dat {
		st.dat[i] = st.id()
	}
	for i := 0; i < st.n; i++ {
		st.dat[st.size+i] = f(i)
	}
	for i := st.size - 1; i >= 1; i-- {
		st.update(i)
	}
}

func (st *SegTree[T]) Get(i int) T {
	return st.dat[st.size+i]
}

func (st *SegTree[T]) GetAll() []T {
	res := make([]T, st.n)
	copy(res, st.dat[st.size:st.size+st.n])
	return res
}

func (st *SegTree[T]) update(i int) {
	st.dat[i] = st.op(st.dat[2*i], st.dat[2*i+1])
}

func (st *SegTree[T]) Set(i int, x T) {
	if i < 0 || i >= st.n {
		return
	}
	i += st.size
	st.dat[i] = x
	for i >>= 1; i > 0; i >>= 1 {
		st.update(i)
	}
}

// [L, R)
func (st *SegTree[T]) Prod(L, R int) T {
	if !(0 <= L && L <= R && R <= st.n) {
		panic("Prod: invalid range")
	}
	vl, vr := st.id(), st.id()
	L += st.size
	R += st.size
	for L < R {
		if L&1 == 1 {
			vl = st.op(vl, st.dat[L])
			L++
		}
		if R&1 == 1 {
			R--
			vr = st.op(st.dat[R], vr)
		}
		L >>= 1
		R >>= 1
	}
	return st.op(vl, vr)
}
