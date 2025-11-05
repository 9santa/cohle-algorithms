#include <bits/stdc++.h>

using namespace std;

struct TrieNode {
public:
    bool isEndOfWord;
    unordered_map<char, TrieNode*> children;

    TrieNode() : isEndOfWord(false) {}

};

class Trie {
    TrieNode* root;

public:
    Trie() : root(new TrieNode()) {}

    void insert(const string& word) {
        TrieNode* node = root;
        for (char c : word) {
            if(node->children.find(c) == node->children.end()) {
                node->children[c] = new TrieNode();
            }
            node = node->children[c];
        }
        node->isEndOfWord = true;
    }
    
    bool search(const string& word) {
        TrieNode* node = root;
        for(char c : word) {
            if(node->children.find(c) == node->children.end()){
                return false;
            }
            node = node->children[c];
        }
        return node->isEndOfWord;
    }
    
    bool startsWith(const string& prefix) {
        TrieNode* node = root;
        for(char c : prefix) {
            if(node->children.find(c) == node->children.end()) return false;
            node = node->children[c];
        }
        return true;
    }

    void printTrie() {
        if(!root) return;
        
        cout << "Root children: ";
        for(auto &pair : root->children) {
            cout << pair.first << " ";   
        }
        cout << "\n";
    }

    void printChildrenOfPrefix(const string &prefix) {
        TrieNode* node = root;
        for(char c : prefix) {
            if(node->children.find(c) == node->children.end()) {
                cout << "Prefix \"" << prefix << "\" not found in Trie.\n"; 
                return;
            }
            node = node->children[c];
        }
        cout << "Chidlren of node for prefix \"" << prefix << "\": ";
        printChildren(node);
    }


    ~Trie() {
        clear(root);
    }

private:
    void clear(TrieNode* node) {
        for (auto &pair : node->children) clear(pair.second);
        delete node;
    } 

    void printChildren(TrieNode* node) {
        if(!node) return;

        cout << "Chidlren: ";
        for(auto &pair : node->children) {
            cout << pair.first << " ";
        }
        cout << "\n";
    }
};


int main()
{
    ios::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);

    Trie trie;
    trie.insert("hello");
    trie.insert("helium");

    trie.printChildrenOfPrefix("hel");

    cout << boolalpha;
    cout << "Search hello: " << trie.search("hello") << "\n";
    cout << "Search help: " << trie.search("help") << "\n";
    cout << "Starts with hel: " << trie.startsWith("hel") << "\n";
    cout << "Starts with hey: " << trie.startsWith("hey") << "\n";
    cout << "Search lo: " << trie.search("lo") << "\n";


    return 0;
}
