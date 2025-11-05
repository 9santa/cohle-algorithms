#include <bits/stdc++.h>
#include <queue>

using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;

    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
private:
    TreeNode* root;

    TreeNode* insert(TreeNode* node, int val) {
        if(!node) return new TreeNode(val);

        if(val < node->val) {
            node->left = insert(node->left, val);
        } else if(val > node->val) {
            node->right = insert(node->right, val);
        }

        return node;
    }

    TreeNode* find_min(TreeNode* node) {
        while(node && node->left) {
            node = node->left;
        }

        return node;
    }

    TreeNode* find_max(TreeNode* node) {
        while(node && node->right) {
            node = node->right;
        }

        return node;
    }

    TreeNode* remove(TreeNode* node, int val) {
        if(!node) return nullptr;

        if(val < node->val) {
            node->left = remove(node->left, val);
        } else if(val > node->val) {
            node->right = remove(node->right, val);
        } else {
            if(!node->left) {
                TreeNode* temp = node->right;
                delete node;
                return temp;
            } else if(!node->right) {
                TreeNode* temp = node->left;
                delete node;
                return temp;
            } 

            TreeNode* temp = find_min(node->right);
            node->val = temp->val;
            node->right = remove(node->right, temp->val);
        }

        return node;
    }

    bool search(TreeNode* node, int val) {
        if(!node) return false;
        if(node->val == val) return true;
        if(val < node->val) return search(node->left, val);
        return search(node->right, val);
    }

    void destroy(TreeNode* node) {
        if(!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    BinarySearchTree() : root(nullptr) {}
    ~BinarySearchTree() { destroy(root); }

    void insert(int val) { root = insert(root, val); }
    void remove(int val) { root = remove(root, val); }
    bool contains(int val) { return search(root, val); }

    // iterative inorder traversal (in ASC order for BST)
    vector<int> inorder_iterative() {
        vector<int> result;
        stack<TreeNode*> st;
        TreeNode* curr = root;

        while(curr || !st.empty()) {
            while(curr) {
                st.push(curr);
                curr = curr->left;
            }
            curr = st.top(); st.pop();
            result.push_back(curr->val);
            curr = curr->right;
        }

        return result;
    }

    // level-order traversal
    vector<vector<int>> level_order() {
        vector<vector<int>> result;
        if(!root) return result;

        queue<TreeNode*> q;
        q.push(root);

        while(!q.empty()) {
            int size = q.size();
            vector<int> level;
            for(int i = 0; i < size; i++) {
                TreeNode* node = q.front(); q.pop();
                level.push_back(node->val);
                if(node->left) q.push(node->left);
                if(node->right) q.push(node->right);
            }
            result.push_back(level);
        }

        return result;
    }
};


// testing example
int main(void) {
    vector<int> test1 = {5, 3, 7, 2, 4, 6, 8};

    vector<int> test2 = {1,2,3,4,5,6,7};

    vector<int> test3 = {7,6,5,4,3,2,1};

    vector<int> test4 = {8,3,10,1,6,14,4,7,13};


    BinarySearchTree bst;
    for(int val : test4) {
        bst.insert(val);
    }

    vector<int> inorder = bst.inorder_iterative();
    cout << "Inorder: ";
    for(int val : inorder) {
        cout << val << " ";
    }
    cout << "\n";



    return 0;
}
