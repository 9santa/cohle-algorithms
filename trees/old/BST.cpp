#include <bits/stdc++.h>
#include <execution>
#include <queue>
#include <stdexcept>
#include <unordered_map>

using namespace std;

// Binary Tree Structure
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode* parent;   // optional, for some modifications

    TreeNode(int x) : val(x), left(nullptr), right(nullptr), parent(nullptr) {}
};

class BST {
private:
    TreeNode* root;

    // Recursive deletion of binary tree
    void clear(TreeNode* node) {
        if(node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    // Copying binary tree
    TreeNode* copy(TreeNode* other) {
        if(!other) return nullptr;

        TreeNode* new_node = new TreeNode(other->val);
        new_node->left = copy(other->left);
        new_node->right = copy(other->right);

        return new_node;
    }

    

public:
    BST() : root(nullptr) {}

    // Rule of 3: destructor, copy constructor, operator=
    ~BST() {
        clear(root);
    }

    BST(const BST& other) {
        root = copy(other.root);
    }

    BST& operator=(const BST& other) {
        if(this != &other) {
            clear(root);
            root = copy(other.root);
        }
        return *this;
    }

    // Move semantics
    BST(BST&& other) noexcept : root(other.root) {
        other.root = nullptr;
    }

    BST& operator=(BST&& other) noexcept {
        if(this != &other) {
            clear(root);
            root = other.root;
            other.root = nullptr;
        }
        return *this;
    }

    void insert(int value) {
        if(!root) {
            root = new TreeNode(value);
            return;
        }

        TreeNode* current = root;
        TreeNode* parent = nullptr;

        while(current) {
            parent = current;
            if(value < current->val) {
                current = current->left;
            } else if(value > current->val) {
                current = current->right;
            } else {
                return; // duplicate (already in the tree)
            }
        }

        if(value < parent->val) {
            parent->left = new TreeNode(value);
        } else {
            parent->right = new TreeNode(value);
        }
    }

    void remove(int value) {
        root = removeNode(root, value);
    }

    bool search(int value) const {
        TreeNode* current = root;
        while(current) {
            if(value == current->val) {
                return true;
            } else if(value < current->val) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return false;
    }

    int find_min() const {
        if(!root) throw std::runtime_error("Tree is empty\n");

        TreeNode* current = root;
        while(current->left) {
            current = current->left;
        }
        return current->val;
    }


    int find_max() const {
        if(!root) throw std::runtime_error("Tree is empty\n");

        TreeNode* current = root;
        while(current->right) {
            current = current->right;
        }
        return current->val;
    }

    int get_height() const {
        if(!root) return 0;

        std::queue<TreeNode*> q;
        q.push(root);
        int height = 0;

        while(!q.empty()) {
            int level_size = q.size();
            height++;

            for(int i = 0; i < level_size; i++) {
                TreeNode* current = q.front();
                q.pop();

                if(current->left) q.push(current->left);
                if(current->right) q.push(current->right);
            }
        }

        return height;
    }

    int size() const {
        if(!root) return 0;

        std::queue<TreeNode*> q;
        q.push(root);
        int count = 0;

        while(!q.empty()) {
            TreeNode* current = q.front();
            q.pop();
            count++;

            if(current->left) q.push(current->left);
            if(current->right) q.push(current->right);
        }
        
        return count;
    }

    bool is_empty() const {
        return root == nullptr;
    }

private:
    TreeNode* removeNode(TreeNode* node, int value) {
        if(!node) return nullptr;

        if(value < node->val) {
            node->left = removeNode(node->left, value);
        } else if(value > node->val) {
            node->right = removeNode(node->right, value);
        } else {
            // no children (leaf node)
            if(!node->left && !node->right) {
                delete node;
                return nullptr;
            } else if(!node->left) {
                TreeNode* temp = node->right;
                delete node;
                return temp;
            } else if(!node->right) {
                TreeNode* temp = node->left;
                delete node;
                return temp;
            } else {
                // min_right = inorder successor
                TreeNode* min_right = node->right;
                while(min_right->left) {
                    min_right = min_right->left;
                }
                node->val = min_right->val;
                node->right = removeNode(node->right, min_right->val);
            }
        }
        return node;
    }
};

int isBalancedHelper(TreeNode *node, bool &isBalancedTree) {
    if (!node || !isBalancedTree)
      return 0;
    
    int left = isBalancedHelper(node->left, isBalancedTree);
    int right = isBalancedHelper(node->right, isBalancedTree);

    if (abs(left - right) > 1)
      isBalancedTree = false;
    return 1 + max(left, right); // returns max height of a tree
}

// Check if a Binary Tree is Balanced
bool isBalanced(TreeNode *root) {
    bool isBalancedTree = true;
    isBalancedHelper(root, isBalancedTree);

    return isBalancedTree;
}

// Check if a Binary Tree is a Binary Search Tree
bool isValidBSTHelper(TreeNode *node, long min_val, long max_val) {
    if (!node)
      return true;

    if (node->val <= min_val || node->val >= max_val)
      return false;

    return isValidBSTHelper(node->left, min_val, node->val) &&
           isValidBSTHelper(node->right, node->val, max_val);
}

bool isValidBST(TreeNode *root) {
    return isValidBSTHelper(root, LONG_MIN, LONG_MAX);
}

// First Common Ancestor of 2 nodes in a binary tree
TreeNode *lowestCommonAncestor(TreeNode *root, TreeNode *first, TreeNode *second) {
    if(!root || root == first || root == second) return root;

    TreeNode *left = lowestCommonAncestor(root->left, first, second);
    TreeNode *right = lowestCommonAncestor(root->right, first, second);

    if(left && right) return root;  // first and second in different subtrees
    if(left) return left;           // both in left subtree
    return right;                   // both in right subtree
}

// Iterative First Common Ancestor of 2 nodes in a binary tree
TreeNode *lowestCommonAncestorIterative(TreeNode* root, TreeNode* p, TreeNode* q) {
    unordered_map<TreeNode*, TreeNode*> parent;
    parent[root] = nullptr;

    stack<TreeNode*> st;
    st.push(root);

    while(!parent.count(p) || !parent.count(q)) {
        TreeNode* node = st.top();
        st.pop();

        if(node->left) {
            parent[node->left] = node;
            st.push(node->left);
        }

        if(node->right) {
            parent[node->right] = node;
            st.push(node->right);
        }
    }

    // Find all ancestors of p
    unordered_set<TreeNode*> ancestors;
    while(p) {
        ancestors.insert(p);
        p = parent[p];
    }

    // Find first intersection
    while(!ancestors.count(q)) {
        q = parent[q];
    }

    return q;
}

// Minimal BST from sorted array
TreeNode *createMinimalBST(const vector<int> &nums, int start, int end) {
    if (start > end)
      return nullptr;

    // To create BST with min height we should use middle element of an array
    int N = nums.size();
    int mid = start + (end - start) / 2; // Avoids overflow

    TreeNode *root = new TreeNode(nums[mid]);

    root->left = createMinimalBST(nums, start, mid - 1);
    root->right = createMinimalBST(nums, mid + 1, end);

    return root;
}

// Wrapper function (with 1 argument, for the 1st call)
TreeNode *createMinimalBST(const vector<int> &nums) {
    return createMinimalBST(nums, 0, nums.size() - 1);
}
