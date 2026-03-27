package gol

type trieNode struct {
	end    bool
	link   [26]int
	cnt    int
	endCnt int // count of words ending in the subtree of this node
}

func newTrieNode() trieNode {
	node := trieNode{}
	for i := range node.link {
		node.link[i] = -1
	}
	return node
}

type Trie struct {
	nodes []trieNode
}

func NewTrie() *Trie {
	return &Trie{nodes: []trieNode{newTrieNode()}}
}

// O(|s|)
func (t *Trie) AddString(s string) {
	v := 0
	t.nodes[v].cnt++
	for _, c := range s {
		idx := int(c - 'a')
		if t.nodes[v].link[idx] == -1 {
			t.nodes = append(t.nodes, newTrieNode())
			t.nodes[v].link[idx] = len(t.nodes) - 1
		}
		v = t.nodes[v].link[idx]
		t.nodes[v].cnt++
	}
	t.nodes[v].end = true
	t.nodes[v].endCnt++
}

// O(|s|)
func (t *Trie) FindString(s string) bool {
	v := 0
	for _, c := range s {
		nxt := t.nodes[v].link[int(c-'a')]
		if nxt == -1 {
			return false
		}
		v = nxt
	}
	return t.nodes[v].end
}

// O(|s|)
func (t *Trie) RemoveString(s string) bool {
	if !t.FindString(s) {
		return false
	}

	v := 0
	t.nodes[v].cnt--
	for _, c := range s {
		nxt := t.nodes[v].link[int(c-'a')]
		v = nxt
		t.nodes[v].cnt--
	}

	t.nodes[v].end = false
	t.nodes[v].endCnt--
	return true
}

// O(26 * |res|)
func (t *Trie) KthString(k int) string {
	res := make([]byte, 0)
	v := 0

	if k <= 0 || k > t.nodes[v].cnt {
		return ""
	}

	for {
		if t.nodes[v].endCnt >= k {
			return string(res)
		}

		k -= t.nodes[v].endCnt

		for c := 0; c < 26; c++ {
			nxt := t.nodes[v].link[c]
			if nxt == -1 {
				continue
			}

			if t.nodes[nxt].cnt >= k {
				res = append(res, byte('a'+c))
				v = nxt
				break
			}

			k -= t.nodes[nxt].cnt
		}
	}
}
