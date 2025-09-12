#include <bits/stdc++.h>

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
};

int getHeight(TreeNode *node, bool &isBalancedTree) {
    if (!node || !isBalancedTree)
      return 0;
    
    int left = getHeight(node->left, isBalancedTree);
    int right = getHeight(node->right, isBalancedTree);

    if (abs(left - right) > 1)
      isBalancedTree = false;
    return 1 + max(left, right); // returns max height of a tree
}

// Check if a Binary Tree is Balanced
bool isBalanced(TreeNode *root) {
    bool isBalancedTree = true;
    getHeight(root, isBalancedTree);

    return isBalancedTree;
}

// Check if a Binary Tree is a Binary Search Tree
bool isValidBSTHelper(TreeNode *node, long min_val, long max_val) {
    if (!node)
      return true;

    if (node->left->val <= min_val || node->right->val >= max_val)
      return false;

    return isValidBSTHelper(node->left, min_val, node->val) &&
           isValidBSTHelper(node->right, node->val, max_val);
}
bool isValidBST(TreeNode *root) {
    return isValidBSTHelper(root, LONG_MIN, LONG_MAX);
}

// First Common Ancestor of 2 nodes in a binary tree
TreeNode *lowestCommonAncestor(TreeNode *root, TreeNode *first, TreeNode *second) {
    if (root && root != first && root != second) {
      TreeNode *left = lowestCommonAncestor(root->left, first, second);
      TreeNode *right = lowestCommonAncestor(root->right, first, second);
    }

    return root; // root is the LCA
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
