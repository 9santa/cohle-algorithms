#pragma once
#include <bits/stdc++.h>
using namespace std;

/** C++14-compatible chunked arena allocator with a free list. Space: O(allocated chunks). */
template<class Node, int CHUNK_BITS = 12> // 2^12 = 4096 slots per chunk
struct NodePool {
    static constexpr int CHUNK_SIZE = 1 << CHUNK_BITS;

    struct Slot {
        union {
            Slot* next;
            typename std::aligned_storage<sizeof(Node), alignof(Node)>::type storage;
        };
    };

    vector<Slot*> chunks;
    Slot* cur = nullptr;
    int cur_used = 0;
    Slot* free_head = nullptr;

    NodePool() { add_chunk(); }

    ~NodePool() {
        for (Slot* p : chunks) ::operator delete((void*)p);
    }

    NodePool(const NodePool&) = delete;
    NodePool& operator=(const NodePool&) = delete;

    /** Resets allocation pointers without destroying live nodes. Time: O(1). */
    void reset_keep_memory() {
        free_head = nullptr;
        if (!chunks.empty()) cur = chunks[0], cur_used = 0;
    }

    /** Frees all chunks and allocates a fresh first chunk. Time: O(number of chunks). */
    void release() {
        for (Slot* p : chunks) ::operator delete((void*)p);
        chunks.clear();
        cur = nullptr;
        cur_used = 0;
        free_head = nullptr;
        add_chunk();
    }

    /** Ensures capacity for at least n nodes. Time: O(extra chunks). */
    void reserve_nodes(int n) {
        int need_chunks = (n + CHUNK_SIZE - 1) / CHUNK_SIZE;
        while ((int)chunks.size() < need_chunks) add_chunk();
    }

    /** Constructs a node in pooled storage. Amortized time: O(1). */
    template<class... Args>
    Node* create(Args&&... args) {
        Slot* s = acquire_slot();
        return ::new ((void*)s) Node(std::forward<Args>(args)...); // union starts at offset 0
    }

    /** Clones x into pooled storage, or returns nullptr for nullptr. Amortized time: O(1). */
    Node* clone(const Node* x) {
        if (!x) return nullptr;
        Slot* s = acquire_slot();
        return ::new ((void*)s) Node(*x);
    }

    /** Destroys x and returns its slot to the free list. Time: O(1). */
    void destroy(Node* x) {
        if (!x) return;
        x->~Node();
        Slot* s = (Slot*)x;
        s->next = free_head;
        free_head = s;
    }

private:
    void add_chunk() {
        Slot* mem = (Slot*)::operator new(sizeof(Slot) * CHUNK_SIZE);
        chunks.push_back(mem);
        cur = mem;
        cur_used = 0;
    }

    Slot* acquire_slot() {
        if (free_head) {
            Slot* s = free_head;
            free_head = free_head->next;
            return s;
        }
        if (cur_used == CHUNK_SIZE) add_chunk();
        return &cur[cur_used++];
    }
};
