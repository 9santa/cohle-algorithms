/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    vector<TreeNode*> generateTrees(int n) {
        if(n == 0) return {};
        map<pair<int, int>, vector<TreeNode*>> memo;
        return generate(1, n, memo);
    }

private:
    vector<TreeNode*> generate(int start, int end, map<pair<int, int>, vector<TreeNode*>>& memo) {
        vector<TreeNode*> result;

        if(start > end) {
            result.push_back(nullptr);
            return result;
        }

        pair<int, int> key = {start, end};
        if(memo.find(key) != memo.end()) {
            return memo[key];
        }

        for(int i = start; i <= end; i++) {
            vector<TreeNode*> left_trees = generate(start, i-1, memo);
            vector<TreeNode*> right_trees = generate(i+1, end, memo);

            for(TreeNode* l : left_trees) {
                for(TreeNode* r : right_trees) {
                    TreeNode* root = new TreeNode(i);
                    root->left = l;
                    root->right = r;
                    result.push_back(root);
                }
            }
        }

        memo[key] = result;
        return result;
    }
};
