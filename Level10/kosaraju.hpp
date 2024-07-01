#include <list>
#include <vector>
#include <stack>
#include <iostream>
#pragma once

using namespace std;

namespace kosaraju {
        
    void waitForAbove50Signal();

    string createNewGraph(vector<list<int>> &adj);

    void dfs1(int v, vector<list<int>>& adj, vector<bool>& visited, stack<int>& Stack);

    void dfs2_list(int v, vector<list<int>>& adj, vector<bool>& visited, list<int>& component);

    string kosaraju_list(int n, vector<list<int>>& adj);

    string handle_client_command(vector<list<int>>& adj,string command);
}

