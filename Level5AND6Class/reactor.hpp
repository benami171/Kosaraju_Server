#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <map>
#include "kosaraju.hpp"
#define PORT 9034
#pragma once


using namespace std;

class Reactor {
    // Shared Graph
    vector<list<int>> adj;
    // Poll list
    struct pollfd *pfds;
    // Current fds in pfds
    int fd_count;
    // Size of max fds to insert
    int fd_size;
    // run poll loop
    bool run;
    typedef void * (*reactorFunc) (Reactor* reactor,int fd);
    // map fds->function
    map<int,reactorFunc > function_map;

public:

    Reactor();
    void* startReactor() ;
    int addFdToReactor(int fd, reactorFunc func);
    int removeFdFromReactor(int fd);
    int stopReactor();
     static void *handle_client(Reactor* reactor,int fd) ;
    static void* handle_connection(Reactor* reactor,int fd);
};