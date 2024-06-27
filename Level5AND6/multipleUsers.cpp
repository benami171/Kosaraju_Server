#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <vector>
#include <stack>
#include <list>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>

#include "reactor.hpp"

using namespace std;

#define PORT "9034"  // Port we're listening on

Reactor reactor;
// Global graph data structure
vector<list<int>> adj;

void createNewGraph(vector<list<int>>  &adj1){
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

void dfs1(int v, vector<list<int>>& adj, vector<bool>& visited, stack<int>& Stack) {
    visited[v] = true;
    for (int u : adj[v]) {
        if (!visited[u]) {
            dfs1(u, adj, visited, Stack);
        }
    }
    Stack.push(v);
}

void dfs2_list(int v, vector<list<int>>& adj, vector<bool>& visited, list<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int u : adj[v]) {
        if (!visited[u]) {
            dfs2_list(u, adj, visited, component);
        }
    }
}

void kosaraju_list(int n, vector<list<int>>& adj) {
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

void* handle_client_command(int client_fd) {
    char buf[256];
    int nbytes = recv(client_fd, buf, sizeof(buf), 0);
    if (nbytes <= 0) {
        if (nbytes == 0) {
            printf("pollserver: socket %d hung up\n", client_fd);
        } else {
            perror("recv");
        }
        close(client_fd);
        // Need to remove client_fd from the reactor here
    } else {
        buf[nbytes] = '\0';
        string command(buf);
        // Use dup2 to redirect stdin to the client's socket
        if (dup2(client_fd, STDIN_FILENO) == -1) {
            perror("dup2");
            close(client_fd);
            return nullptr;
        }
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
}

void * get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void * handle_new_connection(int listener_fd) {
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof(remoteaddr);
    char remoteIP[INET6_ADDRSTRLEN];
    
    int newfd = accept(listener_fd, (struct sockaddr*)&remoteaddr, &addrlen);
    if (newfd == -1) {
        perror("accept");
        return nullptr;
    }
    
    printf("pollserver: new connection from %s on socket %d\n",
           inet_ntop(remoteaddr.ss_family,
                     get_in_addr((struct sockaddr*)&remoteaddr),
                     remoteIP, INET6_ADDRSTRLEN), newfd);
    
    reactor.addFdToReactor(newfd,handle_client_command);
}

int get_listener_socket() {
    int listener;  // Listening socket descriptor
    struct addrinfo hints, *ai, *p;
    int yes = 1;
    int rv;
    
    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        return -1;
    }
    
    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }
        
        // Lose the "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        
        break;
    }
    
    // If we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        return -1;
    }
    
    freeaddrinfo(ai);  // All done with this
    
    // Listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        return -1;
    }
    
    return listener;
}

int main(void) {
    int listener = get_listener_socket();
    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    // Add the listener to the reactor
    reactor.addFdToReactor(listener, handle_new_connection);

    // Start the reactor
    reactor.startReactor();

    return 0;
}
