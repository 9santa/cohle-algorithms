#include <bits/stdc++.h>
using namespace std;

struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

struct List {
private:
    Node* head;
    int size;

public:
    List() : head(nullptr), size(0) {}

    void insert_after_x(int x, int y) {
        if (x == 0) {
            Node* new_node = new Node(y);
            new_node->next = head;
            head = new_node;
            size++;
            return;
        }


        Node* cur = head;
        for (int i = 1; i < x; i++) cur = cur->next;
        Node* new_node = new Node(y);
        new_node->next = cur->next;
        cur->next = new_node;
        size++;
    }

    int get_at_pos(int x) const {
        Node* cur = head;
        for (int i = 1; i < x; i++) cur = cur->next;
        return cur->data;
    }

    void delete_at_pos(int x) {
        if (x == 1) {
            Node* temp = head;
            head = head->next;
            delete temp;
            size--;
            return;
        }

        Node* cur = head;
        for (int i = 1; i < x-1; i++) cur = cur->next;
        Node* temp = cur->next;
        cur->next = temp->next;
        delete temp;
        size--;
    }
};

signed main(void) {
    List linky;
    int q; cin >> q;
    while (q--) {
        int type; cin >> type;
        if (type == 1) {
            int x, y; cin >> x >> y;
            linky.insert_after_x(x, y);
        } else if (type == 2) {
            int x; cin >> x;
            cout << linky.get_at_pos(x) << "\n";
        } else {
            int x; cin >> x;
            linky.delete_at_pos(x);
        }
    }


    return 0;
}
