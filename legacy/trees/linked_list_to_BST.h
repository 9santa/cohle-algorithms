/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
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
    TreeNode* createMinimalBST(const vector<int>& nums, int start, int end) {
        if(start > end) return nullptr;

        int N = (int)nums.size();
        int mid = start+(end-start)/2;

        TreeNode* root = new TreeNode(nums[mid]);

        root->left = createMinimalBST(nums, start, mid - 1);
        root->right = createMinimalBST(nums, mid + 1, end);

        return root;
    }

    TreeNode* sortedListToBST(ListNode* head) {
        if(!head) return nullptr;

        vector<int> nums;
        nums.push_back(head->val);

        while(head && head->next) {
            head = head->next;
            nums.push_back(head->val);
        }

        return createMinimalBST(nums, 0, nums.size()-1);

    }
};

class Solution {
public:
    TreeNode* sortedListToBST(ListNode* head) {
        if(!head) return nullptr;

        vector<int> nums;
        while(head) {
            nums.push_back(head->val);
            head = head->next;
        }

        queue<tuple<TreeNode*, int, int>> q;
        TreeNode* root = new TreeNode(0);
        q.push({root, 0, nums.size()-1});

        while(!q.empty()) {
            auto [node, left, right] = q.front();
            q.pop();

            int mid = left+(right-left)/2;
            node->val = nums[mid];

            if(left <= mid-1) {
                node->left = new TreeNode(0);
                q.push({node->left, left, mid-1});
            }

            if(right >= mid+1) {
                node->right = new TreeNode(0);
                q.push({node->right, mid+1, right});
            }
        }

        return root;
    }
};
