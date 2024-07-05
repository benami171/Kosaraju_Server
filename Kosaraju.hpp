#include <list>
#include <vector>
#include <stack>
#include <iostream>
#pragma once

using namespace std;

namespace Kosaraju {
        
    void* waitForAbove50Signal(int);

    string createNewGraph(vector<list<int>> &adj);

    void dfs1(int v, vector<list<int>>& adj, vector<bool>& visited, stack<int>& Stack);

    void dfs2_list(int v, vector<list<int>>& adj, vector<bool>& visited, list<int>& component);

    string Kosaraju_list(int n, vector<list<int>>& adj);

    string handle_client_command(vector<list<int>>& adj,string command);
}

