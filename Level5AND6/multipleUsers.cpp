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

// Global graph data structure
vector<vector<int>> adj;

void createNewGraph(vector<vector<int>> &adj1){
    int n,m;
    cin >> n >> m;
        adj1.assign(n + 1, vector<int>());
        for (int i = 0; i < m; ++i) {
            int u, v;
            cin >> u >> v;
            adj1[u].push_back(v);
        }
            cout << "Graph updated with " << n << " nodes and " << m << " edges." << endl;
} 

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

void kosaraju_list(int n,vector<vector<int>>&adj1) {
    stack<int> Stack;
    vector<bool> visited(n + 1, false);

    for (int i = 1; i <= n; ++i) {
        if (!visited[i]) {
            dfs1(i, adj1, visited, Stack);
        }
    }

    vector<vector<int>> adjRev(n + 1);
    for (int v = 1; v <= n; ++v) {
        for (int u : adj1[v]) {
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

void handle_client_command(string command) {
    if (command == "Newgraph\n") {
        createNewGraph(adj);
    }
    else if (command == "Kosaraju\n") {
        int n=adj.size()-1;
        kosaraju_list(n,adj);
    } 
    else if (command == "Newedge\n") {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        cout << "New edge added between " << u << " and " << v << endl;
        fflush(stdout);
    } 
    else if (command == "Removeedge\n") {
        int u, v;
        cin >> u >> v;
        auto it = find(adj[u].begin(), adj[u].end(), v);
        if (it != adj[u].end()) {
            adj[u].erase(it);
            cout << "Edge removed between " << u << " and " << v << endl;
            fflush(stdout);
        }
    } 
    else {
        cout << "Unknown command: " << command << endl;
        fflush(stdout);
        }
}


// int main(void) {
//     int listener;
//     int newfd;
//     struct sockaddr_storage remoteaddr;
//     socklen_t addrlen;
//     char remoteIP[INET6_ADDRSTRLEN];
//     int fd_count = 0;
//     int fd_size = 5;
//     struct pollfd *pfds = (struct pollfd *)malloc(sizeof *pfds * fd_size);
//     char buf[256];
//     listener = get_listener_socket();

//     if (listener == -1) {
//         fprintf(stderr, "error getting listening socket\n");
//         exit(1);
//     }

//     pfds[0].fd = listener;
//     pfds[0].events = POLLIN;

//     fd_count = 1;

//     for (;;) {
//         int poll_count = poll(pfds, fd_count, -1);

//         if (poll_count == -1) {
//             perror("poll");
//             exit(1);
//         }

//         for (int i = 0; i < fd_count; i++) {
//             if (pfds[i].revents & POLLIN) {
//                 if (pfds[i].fd == listener) {
//                     addrlen = sizeof remoteaddr;
//                     newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

//                     if (newfd == -1) {
//                         perror("accept");
//                     } 
//                     else {
//                         add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

//                         printf("pollserver: new connection from %s on socket %d\n",
//                                inet_ntop(remoteaddr.ss_family,
//                                          get_in_addr((struct sockaddr *)&remoteaddr),
//                                          remoteIP, INET6_ADDRSTRLEN),newfd);
//                     }
//                 } 
//                 else {
//                     int client_fd = pfds[i].fd;

//                     int nbytes = recv(client_fd, buf, sizeof buf, 0);

//                     if (nbytes <= 0) {
//                         if (nbytes == 0) {
//                             printf("pollserver: socket %d hung up\n", client_fd);
//                         } 
//                         else {
//                             perror("recv");
//                         }

//                         close(client_fd);
//                         del_from_pfds(pfds, i, &fd_count);
//                     } 
//                     else {
//                         buf[nbytes] = '\0';
//                         string command(buf);
//                         // Use dup2 to redirect stdin to the client's socket
//                         if (dup2(client_fd, STDIN_FILENO) == -1) {
//                             perror("dup2");
//                             //close(client_fd);
//                             continue;
//                         }
//                         handle_client_command(command);
//                     }
//                 }
//             }
//         }
//     }
//     free(pfds);
//     return 0;
// }
int main() {
    // Initialize the Reactor
    Reactor reactor;

    // Start the reactor
    reactor.start();

    // Function to handle incoming commands
    reactor.setHandler([&](int fd) {
        char buf[256];
        int nbytes = read(fd, buf, sizeof(buf));
        if (nbytes <= 0) {
            if (nbytes == 0) {
                cout << "Client disconnected." << endl;
            } else {
                perror("read");
            }
            reactor.removeFd(fd);
            close(fd);
        } else {
            buf[nbytes] = '\0';
            string command(buf);
            handle_client_command(command);
        }
    });

    // Set up listening on PORT "9034"
    int listener = get_listener_socket();
    if (listener == -1) {
        cerr << "Error getting listening socket." << endl;
        return 1;
    }
    if (!reactor.addFd(listener, EPOLLIN)) {
        cerr << "Failed to add listener to reactor." << endl;
        return 1;
    }

    // Main event loop
    while (true) {
        reactor.handleEvents();
    }

    // Stop the reactor (this won't be reached in this implementation)
    reactor.stop();

    return 0;
}