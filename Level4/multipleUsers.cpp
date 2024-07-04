#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <stack>
#include <vector>

using namespace std;

#define PORT "9034"  // Port we're listening on

// Global graph data structure
vector<list<int>> adj;

void createNewGraph(vector<list<int>> &adj1) {
    int n, m;
    cin >> n >> m;
    adj.assign(n + 1, list<int>());
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
    }
    cout << "Graph updated with " << n << " nodes and " << m << " edges." << endl;
}
void dfs1(int v, vector<list<int>> &adj, vector<bool> &visited, stack<int> &Stack) {
    visited[v] = true;
    for (int u : adj[v]) {
        if (!visited[u]) {
            dfs1(u, adj, visited, Stack);
        }
    }
    Stack.push(v);
}

void dfs2_list(int v, vector<list<int>> &adj, vector<bool> &visited, list<int> &component) {
    visited[v] = true;
    component.push_back(v);
    for (int u : adj[v]) {
        if (!visited[u]) {
            dfs2_list(u, adj, visited, component);
        }
    }
}

void kosaraju_list(int n, vector<list<int>> &adj) {
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

void handle_client_command(string command) {
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

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// Return a listening socket
int get_listener_socket(void) {
    int listener;
    int yes = 1;
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai);

    if (p == NULL) {
        return -1;
    }

    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size) {
    if (*fd_count == *fd_size) {
        *fd_size *= 2;
        *pfds = (struct pollfd *)realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN;

    (*fd_count)++;
}

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count) {
    pfds[i] = pfds[*fd_count - 1];
    (*fd_count)--;
}

int main(void) {
    int listener;
    int newfd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    char remoteIP[INET6_ADDRSTRLEN];
    int fd_count = 0;
    int fd_size = 5;
    struct pollfd *pfds = (struct pollfd *)malloc(sizeof *pfds * fd_size);
    char buf[256];
    listener = get_listener_socket();

    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    fd_count = 1;

    for (;;) {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        for (int i = 0; i < fd_count; i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == listener) {
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

                        printf("pollserver: new connection from %s on socket %d\n",
                               inet_ntop(remoteaddr.ss_family,
                                         get_in_addr((struct sockaddr *)&remoteaddr),
                                         remoteIP, INET6_ADDRSTRLEN),
                               newfd);
                    }
                } else {
                    int client_fd = pfds[i].fd;

                    int nbytes = recv(client_fd, buf, sizeof buf, 0);

                    if (nbytes <= 0) {
                        if (nbytes == 0) {
                            printf("pollserver: socket %d hung up\n", client_fd);
                        } else {
                            perror("recv");
                        }

                        close(client_fd);
                        del_from_pfds(pfds, i, &fd_count);
                    } else {
                        buf[nbytes] = '\0';
                        string command(buf);
                        // Use dup2 to redirect stdin to the client's socket
                        if (dup2(client_fd, STDIN_FILENO) == -1) {
                            perror("dup2");
                            close(client_fd);
                            continue;
                        }
                        handle_client_command(command);
                    }
                }
            }
        }
    }
    free(pfds);
    return 0;
}