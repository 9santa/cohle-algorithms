#include <bits/stdc++.h>

using namespace std;

struct TreeNode {
	int val;
	TreeNode* left;
	TreeNode* right;

	TreeNode(int value) : val(value), left(nullptr), right(nullptr) {}
};

class BST {
private:
	TreeNode* root;

public:
	BST() : root(nullptr) {}

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
			} else if(value >current->val) {
				current = current->right;
			} else return;
		}

		if(value < parent->val) {
			parent->left = new TreeNode(value);
		} else {
			parent->right = new TreeNode(value);
		}
	}

	int get_height() const {
		if(!root) return 0;

		queue<TreeNode*> q;
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
};


int main() 
{
	vector<int> a;
	BST bst;

	int number = -1;
	while(number != 0) {
		cin >> number;
		if(number != 0) {
			a.push_back(number);
			bst.insert(number);
		}
	}

	cout << bst.get_height() << endl;
	

	return 0;
}
