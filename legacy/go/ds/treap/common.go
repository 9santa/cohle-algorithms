package treap

import "time"

type splitmix64 struct {
	seed uint64
}

func newSplitMix64() splitmix64 {
	return splitmix64{seed: uint64(time.Now().UnixNano())}
}

func (r *splitmix64) Next() uint32 {
	r.seed += 0x9e3779b97f4a7c15
	x := r.seed
	x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9
	x = (x ^ (x >> 27)) * 0x94d049bb133111eb
	x ^= x >> 31
	return uint32(x)
}

type xorShift128 struct {
	x uint32
	y uint32
	z uint32
	w uint32
}

func newXorShift128() xorShift128 {
	return xorShift128{
		x: 123456789,
		y: 362436069,
		z: 521288629,
		w: uint32(time.Now().UnixNano()) | 1,
	}
}

func (r *xorShift128) Next() uint32 {
	t := r.x ^ (r.x << 1)
	r.x, r.y, r.z = r.y, r.z, r.w
	r.w = (r.w * (r.w >> 19)) ^ (t ^ (t >> 8))
	return r.w
}

func max(a, b int) int {
	if a < b {
		return b
	}
	return a
}
