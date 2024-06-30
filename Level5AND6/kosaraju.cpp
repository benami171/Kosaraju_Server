#include "kosaraju.hpp"

void kosaraju::createNewGraph(vector<list<int>> &  adj){
    int n,m;
    cin >> n >> m;
    adj.assign(n + 1, list<int>());
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
    }
    cout << "Graph updated with " << n << " nodes and " << m << " edges." << endl;
}

void kosaraju::dfs1(int v, vector<list<int>> & adj, vector<bool>& visited, stack<int>& Stack) {
    visited[v] = true;
    for (int u : adj[v]) {
        if (!visited[u]) {
            dfs1(u, adj, visited, Stack);
        }
    }
    Stack.push(v);
}

void kosaraju::dfs2_list(int v, vector<list<int>> & adj, vector<bool>& visited, list<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int u : adj[v]) {
        if (!visited[u]) {
            dfs2_list(u, adj, visited, component);
        }
    }
}

void kosaraju::kosaraju_list(int n, vector<list<int>> & adj) {
    stack<int> Stack;
    vector<bool> visited(n + 1, false);

    for (int i = 1; i <= n; ++i) {
        if (!visited[i]) {
            dfs1(i, adj, visited, Stack);
        }
    }

    vector<list<int>> adjRev(n + 1);
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

void kosaraju::handle_client_command(vector<list<int>> & adj,string command) {
    if (command == "Newgraph\n") {
        createNewGraph(adj);
    } else if (command == "Kosaraju\n") {
        int n = adj.size() - 1;
        kosaraju_list(n, adj);
    } else if (command == "Newedge\n") {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        cout << "New edge added between " << u << " and " << v << endl;
        fflush(stdout);
    } else if (command == "Removeedge\n") {
        int u, v;
        cin >> u >> v;
        auto it = find(adj[u].begin(), adj[u].end(), v);
        if (it != adj[u].end()) {
            adj[u].erase(it);
            cout << "Edge removed between " << u << " and " << v << endl;
            fflush(stdout);
        }
    } else {
        cout << "Unknown command: " << command << endl;
        fflush(stdout);
    }
}

