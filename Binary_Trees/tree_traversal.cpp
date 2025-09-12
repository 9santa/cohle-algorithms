#include <bits/stdc++.h>

using namespace std;

template <typename T>
class Node {
public:
    T val;
    //vector<Node*> children;
    Node* left;
    Node* right;

    Node(T x) : val(x), left(nullptr), right(nullptr) {}
};

// 3 Main Types of Tree Traversal:

// Inorder traversal: Left -> Root -> Right
template <typename T>
void inOrder(Node<T>* root) {
    if(root != nullptr) {
        inOrder(root->left);
        cout << root->val << " "; //visited(root);
        inOrder(root->right);
    }

    else return;
}

// Preorder traversal: Root -> Left -> Right
template <typename T>
void preOrder(Node<T>* root) {
    if(root != nullptr) {
        cout << root->val << " "; //visited(root);
        preOrder(root->left);
        preOrder(root->right);
    }

    else return;
}

// Postorder traversal: Left -> Right -> Root
template <typename T>
void postOrder(Node<T>* root) {
    if(root != nullptr) {
        postOrder(root->left);
        postOrder(root->right);
        cout << root->val << " "; //visited(root);
    }

    else return;
}

int main()
{
    ios::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);

    Node<int>* root = new Node(1);
    root->left = new Node(12);
    root->right = new Node(9);
    root->left->left = new Node(5);
    root->left->right = new Node(6);

    cout << "Inorder traversal: ";
    inOrder(root);
    cout << "\nPreorder traversal: ";
    preOrder(root);
    cout << "\nPostorder traversal: ";
    postOrder(root);
    cout << endl;


    return 0;
}
