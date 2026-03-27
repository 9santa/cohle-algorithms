
// Edge-biconnected components
vector<vector<int>> components;
vector<int> tin;    // dfs enter time
vector<int> up;
vector<bool> vis;
vector<vector<int>> graph;
int T = 0;

stack<int> st;  // for ordering

void dfs1(int v, int p) {
    vector<int> comp; // component elements

    tin[v] = T++;
    up[v] = tin[v];
    vis[v] = true;
    st.push(v);

    for (auto u : graph[v]) {
        if (u == p) continue;

        if (!vis[u]) {
            dfs1(u, v);
            up[v] = min(up[v], up[u]);
        } else {
            up[v] = min(up[v], tin[u]);
        }
    }

    if (up[v] >= tin[v]) {
        // then edge p-v is a bridge
        while (true) {
            int x = st.top(); st.pop();
            comp.push_back(x);
            if (x == v) break;

        }
        components.push_back(comp);
    }
}


stack<pair<int, int>> st;
vector<vector<pair<int, int>>> components;

void dfs2(int v, int p) {
    vector<pair<int, int>> comp;

    tin[v] = up[v] = T++;
    vis[v] = true;
    bool articulationp = false;
    int cnt = 0;

    for (auto u : graph[v]) {
        if (u == p) continue;

        if (!vis[u]) {
            st.push({v, u});
            dfs2(u, v);
            cnt++;
            up[v] = min(up[v], up[u]);

            if (up[u] >= tin[v]) {
                articulationp = true;
                while (true) {
                    auto edge = st.top(); st.pop();
                    comp.push_back(edge);
                    if (edge == make_pair(u, v)) break;
                }
            }
        } else {
            up[v] = min(up[v], tin[u]);
            if (tin[u] < tin[v]) {
                st.push({u, v});
            }
        }
    }

    if (articulationp || (p == -1 && cnt > 1)) {
        // then v - articulation point
        return;
    }
}
