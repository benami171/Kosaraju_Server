#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>

using namespace std;

void dfs(int v, const vector<vector<int>>& graph, vector<bool>& visited, stack<int>& st) {
    visited[v] = true;
    for (int u : graph[v]) {
        if (!visited[u]) {
            dfs(u, graph, visited, st);
        }
    }
    st.push(v);
}

void dfs_reverse(int v, const vector<vector<int>>& reverse_graph, vector<bool>& visited, vector<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int u : reverse_graph[v]) {
        if (!visited[u]) {
            dfs_reverse(u, reverse_graph, visited, component);
        }
    }
}

vector<vector<int>> kosaraju(int n, const vector<pair<int, int>>& edges) {
    vector<vector<int>> graph(n + 1);
    vector<vector<int>> reverse_graph(n + 1);

    for (auto edge : edges) {
        graph[edge.first].push_back(edge.second);
        reverse_graph[edge.second].push_back(edge.first);
    }

    vector<bool> visited(n + 1, false);
    stack<int> st;

    for (int i = 1; i <= n; ++i) {
        if (!visited[i]) {
            dfs(i, graph, visited, st);
        }
    }

    fill(visited.begin(), visited.end(), false);
    vector<vector<int>> strongly_connected_components;

    while (!st.empty()) {
        int v = st.top();
        st.pop();

        if (!visited[v]) {
            vector<int> component;
            dfs_reverse(v, reverse_graph, visited, component);
            strongly_connected_components.push_back(component);
        }
    }

    return strongly_connected_components;
}

int main() {
    int n, m;
    cin >> n >> m;

    vector<pair<int, int>> edges;
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        edges.emplace_back(u, v);
    }

    auto scc = kosaraju(n, edges);

    cout << "Strongly Connected Components: " << endl;
    for (const auto& component : scc) {
        for (int node : component) {
            cout << node << " ";
        }
        cout << endl;
    }

    return 0;
}