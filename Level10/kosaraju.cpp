#include "kosaraju.hpp"

#include <stdio.h>

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;       // Global mutex for synchronization
pthread_cond_t cv_above50 = PTHREAD_COND_INITIALIZER;  // Condition variable for above 50%
bool current_above50_state = false;


void kosaraju::waitForAbove50Signal() {
    pthread_mutex_lock(&mtx);
    while (true) {
        pthread_cond_wait(&cv_above50, &mtx);
        if (current_above50_state) {
            cout << "At Least '50%' of the graph belongs to the same SCC\n"
                 << endl;
        } else {
            cout << "At Least '50%' of the graph does not belongs to the same SCC\n"
                 << endl;
        }
    }
    pthread_mutex_unlock(&mtx);
}

string kosaraju::createNewGraph(vector<list<int>>& adj) {
    string ans;
    int n, m;
    ans += "define graph\n";
    cin >> n >> m;
    adj.assign(n + 1, list<int>());
    for (int i = 0; i < m; ++i) {
        int u, v;
        ans += "addEdge\n";
        cin >> u >> v;
        adj[u].push_back(v);
    }
    ans += "Graph updated with " + to_string(n) + " nodes and " + to_string(m) + " edges.\n";
    return ans;
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

string kosaraju::kosaraju_list(int n, vector<list<int>>& adj) {
    string ans;
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
    int max_scc_size = 0;
    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            list<int> component;
            dfs2_list(v, adjRev, visited, component);
            max_scc_size = max(max_scc_size, static_cast<int>(component.size())); // Getting the highest component size out of all the components.
            for (int u : component) {
                ans += to_string(u) + " ";
            }
            ans += "\n";
        }
    }
    bool new_above50_state = (max_scc_size >= n/2); // If there is a scc component with size above 50% or not.

    pthread_mutex_lock(&mtx);
    if (new_above50_state != current_above50_state) { // If there was a change in the graph state from the last time Kosaraju was called
        current_above50_state = new_above50_state; 
        pthread_cond_signal(&cv_above50);
    }
    pthread_mutex_unlock(&mtx);

    return ans;
}

string kosaraju::handle_client_command(vector<list<int>>& adj, string command) {
    string ans;
    if (command == "Newgraph\n") {
        ans += "Creating new graph\n";
        ans += createNewGraph(adj);
    } else if (command == "Kosaraju\n") {
        int n = adj.size() - 1;
        ans = kosaraju_list(n, adj);
    } else if (command == "Newedge\n") {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        ans += "New edge added between " + to_string(u) + " and " + to_string(v) + "\n";
        fflush(stdout);
    } else if (command == "Removeedge\n") {
        int u, v;
        cin >> u >> v;
        auto it = find(adj[u].begin(), adj[u].end(), v);
        if (it != adj[u].end()) {
            adj[u].erase(it);
            ans += "Edge removed between " + to_string(u) + " and " + to_string(v) + "\n";
            fflush(stdout);
        }
    } else {
        cout << "Unknown command: " << command << endl;
        fflush(stdout);
    }
    return ans;
}
