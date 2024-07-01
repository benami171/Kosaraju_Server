#include "kosaraju.hpp"

#include <stdio.h>

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

mutex mtx;                      // Global mutex for synchronization
condition_variable cv_above50;  // Condition variable for above 50%
bool above50_ready = false;     // Condition flag for above 50%

void kosaraju::waitForAbove50Signal() {
    unique_lock<std::mutex> lck(mtx);
    cv_above50.wait(lck, [] { return above50_ready; });
    while (true) {
        if (above50_ready) {
            cout << "At Least '50%' of the graph belongs to the same SCC\n"
                 << std::endl;
        } else {
            cout << "Not At Least '50%' of the graph belongs to the same SCC\n"
                 << std::endl;
        }
        cv_above50.wait(lck);
    }
}

void kosaraju::createNewGraph(vector<list<int>>& adj) {
    int n, m;
    cout << "define graph" << endl;
    cin >> n >> m;
    adj.assign(n + 1, list<int>());
    for (int i = 0; i < m; ++i) {
        int u, v;
        cout << "addEdge" << endl;
        cin >> u >> v;
        adj[u].push_back(v);
    }
    cout << "Graph updated with " << n << " nodes and " << m << " edges." << endl;
}

void kosaraju::dfs1(int v, vector<list<int>>& adj, vector<bool>& visited, stack<int>& Stack) {
    visited[v] = true;
    for (int u : adj[v]) {
        if (!visited[u]) {
            dfs1(u, adj, visited, Stack);
        }
    }
    Stack.push(v);
}

void kosaraju::dfs2_list(int v, vector<list<int>>& adj, vector<bool>& visited, list<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int u : adj[v]) {
        if (!visited[u]) {
            dfs2_list(u, adj, visited, component);
        }
    }
}

void kosaraju::kosaraju_list(int n, vector<list<int>>& adj) {
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
    bool above50 = false;
    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            list<int> component;
            dfs2_list(v, adjRev, visited, component);
            if (component.size() >= (size_t)n / 2 && !above50) {
                above50 = true;
                std::lock_guard<std::mutex> lk(mtx);  // Lock the mutex
                above50_ready = true;                 // Set the condition for above 50%
                cv_above50.notify_one();              // Signal the above 50% condition variable
            }
            for (int u : component) {
                cout << u << " ";
            }
            cout << endl;
        }
    }
    if (!above50) {
        std::lock_guard<std::mutex> lk(mtx);  // Lock the mutex
        above50_ready = false;                // Reset the condition for above 50%
        cv_above50.notify_one();              // Signal the above 50% condition variable}
    }
}

void kosaraju::handle_client_command(vector<list<int>>& adj, string command) {
    if (command == "Newgraph\n") {
        cout << "Creating new graph" << endl;
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
