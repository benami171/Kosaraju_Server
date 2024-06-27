#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <vector>
#include <functional>
#include <iostream>
#include <unordered_map>

typedef void * (* reactorFunc) (int fd);
//typedef reactor;
class Reactor {
public:
    Reactor();
    ~Reactor();
    void* startReactor();
    int stopReactor();
    int addFdToReactor(int fd, reactorFunc func);
    int removeFdFromReactor( int fd);

private:
    int fd_count;
    int fd_size;
    bool running;
    struct pollfd *pfds;
    std::unordered_map<int,reactorFunc> fdMap;
};

#endif // REACTOR_HPP
