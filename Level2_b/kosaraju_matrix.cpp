#include <iostream>
#include <vector>
#include <stack>
#include <list>
using namespace std;

void dfs1(int v, vector<vector<int>>& adj, vector<bool>& visited, stack<int>& Stack) {
    visited[v] = true;
    for (size_t u = 1; u < adj[v].size(); ++u) { // size_t for comparison with adj[v].size()
        if (adj[v][u] && !visited[u]) {
            dfs1(u, adj, visited, Stack);
        }
    }
    Stack.push(v);
}

void dfs2_matrix(int v, vector<vector<int>>& adj, vector<bool>& visited, list<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (size_t u = 1; u < adj[v].size(); ++u) { // size_t for comparison with adj[v].size()
        if (adj[v][u] && !visited[u]) {
            dfs2_matrix(u, adj, visited, component);
        }
    }
}

void kosaraju_matrix(int n, vector<vector<int>>& adj) {
    stack<int> Stack;
    vector<bool> visited(n + 1, false);

    for (int i = 1; i <= n; ++i) {
        if (!visited[i]) {
            dfs1(i, adj, visited, Stack);
        }
    }

    vector<vector<int>> adjRev(n + 1,vector<int>(n+1,0));
    for (int v = 1; v <= n; ++v) {
        for (int u = 1; u <= n; ++u) {
            if (adj[v][u]) {
                adjRev[u][v] = 1;
            }
        }
    }

    fill(visited.begin(), visited.end(), false);
    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            list<int> component;
            dfs2_matrix(v, adjRev, visited, component);
            for (int u : component) {
                cout << u << " ";
            }
            cout << endl;
        }
    }
}

int main() {
    int n, m;
    cin >> n >> m;
    vector<vector<int>> adj(n + 1, vector<int>(n + 1, 0));
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u][v] = 1;
    }

    kosaraju_matrix(n, adj);
    return 0;
}
