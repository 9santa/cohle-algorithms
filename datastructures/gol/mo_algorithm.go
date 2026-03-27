package gol

import "sort"

// Answer static range queries in O((N+Q) * sqrt(N))
// Queries are offline
// Update operation is O(1) or close to that

var MoAlgorithmBlockSize int

type MoAlgorithmQuery struct {
	L   int
	R   int
	Idx int
}

// zig-zag trick
func (q MoAlgorithmQuery) less(rhs MoAlgorithmQuery) bool {
	blockA := q.L / MoAlgorithmBlockSize
	blockB := rhs.L / MoAlgorithmBlockSize

	if blockA != blockB {
		return blockA < blockB // different L-block: only blocks matter
	}

	if blockA&1 == 1 {
		return q.R > rhs.R
	}
	return q.R < rhs.R
}

func RunMoAlgorithm(
	qs []MoAlgorithmQuery,
	add func(int),
	remove func(int),
	getAns func() int,
) []int {
	ans := make([]int, len(qs))
	sort.Slice(qs, func(i, j int) bool { return qs[i].less(qs[j]) })

	// TODO: initialize data structure

	curl := 0
	curr := -1
	// invariant: data structure will always reflect the range [curl, curr]
	for _, q := range qs {
		for curl > q.L {
			curl--
			add(curl)
		}
		for curr < q.R {
			curr++
			add(curr)
		}
		for curl < q.L {
			remove(curl)
			curl++
		}
		for curr > q.R {
			remove(curr)
			curr--
		}
		ans[q.Idx] = getAns()
	}
	return ans
}
