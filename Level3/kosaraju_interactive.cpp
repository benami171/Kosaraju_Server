#include <iostream>
#include <vector>
#include <stack>
#include <list>
#include <algorithm>
using namespace std;

void dfs1(int v, vector<vector<int>>& adj, vector<bool>& visited, stack<int>& Stack) {
    visited[v] = true;
    for (int u : adj[v]) {
        if (!visited[u]) {
            dfs1(u, adj, visited, Stack);
        }
    }
    Stack.push(v);
}

void dfs2_list(int v, vector<vector<int>>& adj, vector<bool>& visited, list<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int u : adj[v]) {
        if (!visited[u]) {
            dfs2_list(u, adj, visited, component);
        }
    }
}

void kosaraju_list(int n, vector<vector<int>>& adj) {
    stack<int> Stack;
    vector<bool> visited(n + 1, false);

    for (int i = 1; i <= n; ++i) {
        if (!visited[i]) {
            dfs1(i, adj, visited, Stack);
        }
    }

    vector<vector<int>> adjRev(n + 1);
    for (int v = 1; v <= n; ++v) {
        for (int u : adj[v]) {
            adjRev[u].push_back(v);
        }
    }

    fill(visited.begin(), visited.end(), false);
    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            list<int> component;
            dfs2_list(v, adjRev, visited, component);
            for (int u : component) {
                cout << u << " ";
            }
            cout << endl;
        }
    }
}

int main() {
    string command;
    int n = 0, m = 0;
    vector<vector<int>> adj;

    while (cin >> command) {
        if (command == "Newgraph") {
            cin >> n >> m;
            adj.assign(n + 1, vector<int>());
            for (int i = 0; i < m; ++i) {
                int u, v;
                cin >> u >> v;
                adj[u].push_back(v);  // Adjusted index to 0-based
            }
        } 
        else if (command == "Kosaraju") {
            kosaraju_list(n, adj);
        } 
        else if (command == "Newedge") {
            int u, v;
            cin >> u >> v;
            adj[u].push_back(v);  // Adjusted index to 0-based
        } 
        else if (command == "Removeedge") {
            int u, v;
            cin >> u >> v;
            // Find and erase the edge
            auto it = find(adj[u].begin(), adj[u].end(), v);
            if (it != adj[u].end()) {
                adj[u].erase(it);
            }
        }
        else exit(1);
    }

    return 0;
}
