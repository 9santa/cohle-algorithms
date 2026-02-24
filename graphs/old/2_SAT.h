#include <bits/stdc++.h>

using namespace std;


class TwoSAT {
private:
    int n;  // number of variables
    vector<vector<int>> graph;  // implication graph
    vector<vector<int>> reversed_graph; // reversed graph for Kosaraju's algorithm

    // first dfs for Kosaraju's
    void dfs1(int node, vector<bool>& visited, stack<int>& order) {
        visited[node] = true;
        for(auto v : graph[node]) {
            if(!visited[v]) {
                dfs1(v, visited, order);
            }
        }
        order.push(node);
    }

    // second dfs for Kosaraju's
    void dfs2(int node, vector<bool>& visited, vector<int>& comp, int comp_id) {
        visited[node] = true;
        comp[node] = comp_id;
        for(auto v : reversed_graph[node]) {
            if(!visited[v]) {
                dfs2(v, visited, comp, comp_id);
            }
        }
    }

public:
    TwoSAT(int variables) : n(variables) {
        graph.resize(2*n);
        reversed_graph.resize(2*n);
    }

    void addClause(int a, int b) {
        int a_var = abs(a) - 1;
        int b_var = abs(b) - 1;

        // for variable x: 2*x = x, 2*x+1 = !x
        int a_true = 2 * a_var;
        int a_false = 2 * a_var + 1;
        int b_true = 2 * b_var;
        int b_false = 2 * b_var + 1;

        // determine true or false based on sign
        int a_literal = (a > 0) ? a_true : a_false;
        int b_literal = (b > 0) ? b_true : b_false;

        // get the negated literals (flip last bit)
        int not_a_literal = a_literal ^ 1;
        int not_b_literal = b_literal ^ 1;

        // implication: !a -> b
        graph[not_a_literal].push_back(b_literal);
        reversed_graph[b_literal].push_back(not_a_literal);

        // implication: !b -> a
        graph[not_b_literal].push_back(a_literal);
        reversed_graph[a_literal].push_back(not_b_literal);
    }


    // solve 2-SAT problem
    vector<bool> solve() {
        // step 1: first dfs to get the order of exit
        vector<bool> visited(2*n, false);
        stack<int> order;

        for(int i = 0; i < 2*n; i++) {
            if(!visited[i]) {
                dfs1(i, visited, order);
            }
        }

        // step 2: second dfs on reversed graph to find SCCs
        std::fill(visited.begin(), visited.end(), false);
        vector<int> comp(2*n, -1);
        int comp_id = 0;

        while(!order.empty()) {
            int node = order.top();
            order.pop();

            if(!visited[node]) {
                dfs2(node, visited, comp, comp_id);
                comp_id++;
            }
        }

        // step 3: check for contradiction and assign values
        vector<bool> assignment(n, false);
        for(int i = 0; i < n; i++) {
            if(comp[2*i] == comp[2*i+1]) {
                puts("NO SOLUTION");
                return {};
            }

            // right SCC should be true, left SCC should be false
            assignment[i] = comp[2*i] > comp[2*i+1];
        }

        return assignment;
    }
};

