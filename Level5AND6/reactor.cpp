#include "reactor.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>

Reactor::Reactor() : epoll_fd(-1) {}

Reactor::~Reactor() {
    if (epoll_fd != -1) {
        close(epoll_fd);
    }
}

bool Reactor::start() {
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        std::cerr << "Failed to create epoll file descriptor." << std::endl;
        return false;
    }
    return true;
}

bool Reactor::stop() {
    if (epoll_fd != -1) {
        close(epoll_fd);
        epoll_fd = -1;
    }
    return true;
}

bool Reactor::addFd(int fd, short events) {
    struct epoll_event event;
    event.events = events;
    event.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        std::cerr << "Failed to add fd to epoll." << std::endl;
        return false;
    }
    return true;
}

bool Reactor::removeFd(int fd) {
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr) == -1) {
        std::cerr << "Failed to remove fd from epoll." << std::endl;
        return false;
    }
    return true;
}

void Reactor::setHandler(std::function<void(int)> handler) {
    event_handler = handler;
}

void Reactor::handleEvents() {
    constexpr int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];

    int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if (num_events == -1) {
        std::cerr << "Error in epoll_wait." << std::endl;
        return;
    }

    for (int i = 0; i < num_events; ++i) {
        int fd = events[i].data.fd;
        event_handler(fd);
    }
}
