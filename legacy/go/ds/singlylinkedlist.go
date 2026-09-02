package gol

type listNode struct {
	data int
	next *listNode
}

type List struct {
	head *listNode
	size int
}

func NewList() *List {
	return &List{}
}

func (l *List) InsertAfterX(x, y int) {
	if x == 0 {
		newNode := &listNode{data: y, next: l.head}
		l.head = newNode
		l.size++
		return
	}

	cur := l.head
	for i := 1; i < x && cur != nil; i++ {
		cur = cur.next
	}
	if cur == nil {
		return
	}

	newNode := &listNode{data: y, next: cur.next}
	cur.next = newNode
	l.size++
}

func (l *List) GetAtPos(x int) (int, bool) {
	cur := l.head
	for i := 1; i < x && cur != nil; i++ {
		cur = cur.next
	}
	if cur == nil {
		return 0, false
	}
	return cur.data, true
}

func (l *List) DeleteAtPos(x int) bool {
	if x <= 0 || l.head == nil {
		return false
	}

	if x == 1 {
		l.head = l.head.next
		l.size--
		return true
	}

	cur := l.head
	for i := 1; i < x-1 && cur != nil; i++ {
		cur = cur.next
	}
	if cur == nil || cur.next == nil {
		return false
	}

	cur.next = cur.next.next
	l.size--
	return true
}

func (l *List) Size() int {
	return l.size
}
