#include <bits/stdc++.h>

using namespace std;

class Node {
public:
    int value;
    Node* next;

    Node(int val) : value(val), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;
    int size;
public:
    LinkedList() : head(nullptr), size(0) {}

    // ~LinkedList() {
    //     clear();
    // }

    void insert_after_x(int x, int y) {
        if(x < 0) return;

        if(x == 0 || head == nullptr)  {
            Node* new_node = new Node(y);
            new_node->next = head;
            head = new_node;
            size++;
            return;
        }

        if(x >= size) {
            Node* cur = head;
            while(cur->next != nullptr) {
                cur = cur->next;
            }
            Node* new_node = new Node(y);
            cur->next = new_node;
            size++;
            return;
        }

        Node* cur = head;

        for(int i = 0; i < x-1; i++) {
            cur = cur->next;
        }

        Node* new_node = new Node(y);
        new_node->next = cur->next;
        cur->next = new_node;
        size++;
    }

    int get_at_pos(int x) {
        if(x < 0 || x >= size) {
            throw out_of_range("out of range");
        }

        if(x == 0) {
            return head->value;
        }

        Node* cur = head;
        for(int i = 0; i < x; i++) {
            cur = cur->next;
        }

        return cur->value;
    }

    void delete_at_pos(int x) {
        if(x == 0) {
            Node* temp = head;
            head = head->next;
            delete temp;
            size--;
            return;
        }

        Node* cur = head;
        for(int i = 0; i < x-1; i++) {
            cur = cur->next;
        }

        Node* temp = cur->next;
        cur->next = temp->next;
        delete temp;
        size--;
    }
};

signed main(void) {
    LinkedList list;
    int q; cin >> q;

    while(q--) {
        int type; cin >> type;

        if(type == 1) {
            int x, y; cin >> x >> y;
            list.insert_after_x(x, y);
        } else if(type == 2) {
            int x; cin >> x;
            int val = list.get_at_pos(x);
            cout << val << "\n";
        } else {
            int x; cin >> x;
            list.delete_at_pos(x);
        }
    }

    return 0;
}
