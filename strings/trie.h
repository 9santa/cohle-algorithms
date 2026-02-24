#include "../header.h"

struct Trie {
    struct Node {
        bool end;
        int link[26];
        int cnt;
        int end_cnt;    // count of words ending in the subtree of this node

        Node() {
            end = false;
            memset(link, -1, sizeof(link));
            cnt = 0;
            end_cnt = 0;
        }
    };

    vector<Node> trie = {Node()};

    // O(|s|)
    void addString(const string& s) {
        int v = 0;
        trie[v].cnt++;
        for (auto c : s) {
            if (trie[v].link[c - 'a'] == -1) {
                trie.emplace_back();
                trie[v].link[c - 'a'] = (int)trie.size()-1;
            }
            v = trie[v].link[c - 'a'];
            trie[v].cnt++;
        }
        trie[v].end = true;
        trie[v].end_cnt++;
    }

    // O(|s|)
    bool findString(const string& s) const {
        int v = 0;
        for (auto c : s) {
            int nxt = trie[v].link[c - 'a'];
            if (nxt == -1) return false;
            v = nxt;
        }
        return trie[v].end;
    }

    // O(|s|)
    bool removeString(const string& s) {
        int v = 0;
        trie[v].cnt--;
        for (auto c : s) {
            int nxt = trie[v].link[c - 'a'];
            if (nxt == -1) return false;    // no such word
            v = nxt;
            trie[v].cnt--;
        }
        if (!trie[v].end) return false; // exists as a prefix, but not as a word

        trie[v].end = false;
        trie[v].end_cnt--;
        return true;
    }

    // O(26 * |res|)
    string kthString(int k) {
        string res;
        int v = 0;

        if (k <= 0 || k > trie[v].cnt) return "";

        while(1) {
            if (trie[v].end_cnt >= k) {
                return res;
            }

            k -= trie[v].end_cnt;

            for (int c = 0; c < 26; c++) {
                int nxt = trie[v].link[c];
                if (nxt == -1) continue;

                if (trie[nxt].cnt >= k) {
                    res.push_back(char('a' + c));
                    v = nxt;
                    break;
                } else k -= trie[nxt].cnt;
            }
        }
    }
};
