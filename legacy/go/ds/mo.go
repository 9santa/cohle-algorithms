package gol

import (
	"math"
	"sort"
)

type IntPair struct {
	First  int
	Second int
}

type Mo struct {
	LR []IntPair // queries [L, R)
}

func (m *Mo) Add(L, R int) {
	m.LR = append(m.LR, IntPair{First: L, Second: R})
}

func GetMoOrder(LR []IntPair) []int {
	N := 1
	for _, p := range LR {
		if p.First > N {
			N = p.First
		}
		if p.Second > N {
			N = p.Second
		}
	}
	Q := len(LR)
	if Q == 0 {
		return nil
	}

	length := int(math.Sqrt(3.0) * float64(N) / math.Sqrt(2.0*float64(Q))) // better block size than just sqrt(N)
	if length < 1 {
		length = 1
	}

	order := make([]int, Q)
	for i := range order {
		order[i] = i
	}

	// zig-zag optimization
	sort.Slice(order, func(i, j int) bool {
		a := order[i]
		b := order[j]
		ablock := LR[a].First / length
		bblock := LR[b].First / length
		if ablock != bblock {
			return ablock < bblock
		}
		if ablock&1 == 1 {
			return LR[a].Second > LR[b].Second
		}
		return LR[a].Second < LR[b].Second
	})

	cost := func(a, b int) int {
		x := LR[order[a]]
		y := LR[order[b]]
		return abs(x.First-y.First) + abs(x.Second-y.Second)
	}

	// optimize queries Manhattan distance
	for k := 0; k < Q-5; k++ {
		if cost(k, k+2)+cost(k+1, k+3) < cost(k, k+1)+cost(k+2, k+3) {
			order[k+1], order[k+2] = order[k+2], order[k+1]
		}
		if cost(k, k+3)+cost(k+1, k+4) < cost(k, k+1)+cost(k+3, k+4) {
			order[k+1], order[k+3] = order[k+3], order[k+1]
		}
	}

	return order
}

func (m *Mo) Calc(
	addL func(int),
	addR func(int),
	rmL func(int),
	rmR func(int),
	query func(int),
) {
	order := GetMoOrder(m.LR)
	l, r := 0, 0
	for _, idx := range order {
		for l > m.LR[idx].First {
			l--
			addL(l)
		}
		for r < m.LR[idx].Second {
			addR(r)
			r++
		}
		for l < m.LR[idx].First {
			rmL(l)
			l++
		}
		for r > m.LR[idx].Second {
			r--
			rmR(r)
		}
		query(idx)
	}
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}
