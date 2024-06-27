#include "reactor.hpp"
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <iostream>

Reactor::Reactor() {
    running = false;
    fd_size = 5;
    fd_count = 0;
    pfds = (struct pollfd*)malloc(sizeof *pfds * fd_size);
    if(!pfds) {
        std::cerr << "Failed to allocate memory for pollfd array" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Reactor::~Reactor() {
    free(pfds);
}

void* Reactor::startReactor() {
    running = true;
    int poll_count; // Number of events that have occurred
    while (running) {
        // Wait for events on the file descriptors.
        poll_count = poll(pfds, fd_count, -1);  // -1 means wait indefinitely
        if (poll_count == -1) {
            perror("poll");
            running = false;  // Stop the reactor on error
            break;
        }

        // Handle events
        for (int i = 0; i < fd_count && poll_count > 0; ++i) { // as long as there are events to process
            if (pfds[i].revents & POLLIN) {  // Check if read event
                auto it = fdMap.find(pfds[i].fd);
                if (it != fdMap.end()) {
                    // Call the associated function
                    it->second(pfds[i].fd);
                }
                --poll_count;  // Decrement the count of remaining events to process
            }
        }
    }
    return nullptr;  // Return nullptr to match the void* return type
}

int Reactor::stopReactor() {
    running = false;
    return 0;
}

int Reactor::addFdToReactor( int newFd, reactorFunc func) {
    //Reactor* r = static_cast<Reactor*>(reactor);
    if (this->fd_count == this->fd_size) {
        this->fd_size *= 2;
        this->pfds = (struct pollfd*)realloc(this->pfds, sizeof(*this->pfds) * this->fd_size);
    }
    this->pfds[this->fd_count].fd = newFd;
    this->pfds[this->fd_count].events = POLLIN;
    this->fdMap[newFd] = func;
    this->fd_count++;
    return 0;
}

// casting the void pointer to a reactor pointer so we can access the members of the reactor class.
// we then iterate through the pfds array to find the fd that we want to remove from the reactor.
// if we find the fd, we replace it with the last element in the array and decrement the fd_count.
// we then remove the fd from the fdMap and return 0.

int Reactor::removeFdFromReactor(int fd) {
    //Reactor* r = static_cast<Reactor*>(reactor);
    for (int i = 0; i < this->fd_count; i++) {
        if (this->pfds[i].fd == fd) {
            this->pfds[i] = this->pfds[this->fd_count - 1];
            this->fd_count--;
            this->fdMap.erase(fd);
            return 0;
        }
    }
    return -1;
}