#include "reactor.hpp"


Reactor::Reactor() {
    // initialize pfds to 5 spots
    fd_size = 5;
    fd_count = 0;
    // create
    pfds = (struct pollfd *) malloc(sizeof *(pfds) * fd_size);
    if (!pfds) {
        perror("poll");
        exit(1);
    }
    run = true;
}

void *Reactor::startReactor() {
    while (run) {
        // poll wait
        int poll_count = poll(pfds, fd_count, -1);
        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }
        for (int i = 0; i < fd_count; i++) {
            // if got hot fds -> run its function
            if (pfds[i].revents & POLLIN) {
                function_map[pfds[i].fd](this,pfds[i].fd);
            }
        }
    }
}

int Reactor::stopReactor()  {
    // stop the run
    run = false;
    // free the poll list
    free(pfds);
    return 0;
}

int Reactor::addFdToReactor(int fd, Reactor::reactorFunc func) {
    // if no room realloc the list
    if (fd_count == fd_size) {
        fd_size *= 2;
        pfds = (struct pollfd *) realloc(pfds, sizeof(*pfds) * fd_size);
    }
    // put fds in next availabe space and update on map
    pfds[fd_count].fd = fd;
    pfds[fd_count].events = POLLIN;
    function_map[fd] = func;
    fd_count++;
    return 0;
}

int Reactor::removeFdFromReactor(int fd) {
    // search for fds in poll list and remove from map
    for (int i = 0; i < fd_count; i++) {
        if (pfds[i].fd == fd) {
            pfds[i] = pfds[fd_count - 1];
            fd_count--;
            function_map.erase(fd);
            close(fd);
            return 0;
        }
    }
    return -1;
}

void *Reactor::handle_client(Reactor *reactor, int fd) {
    char buf[256];
    // receive messge from fd
    int nbytes = recv(fd, buf, sizeof buf, 0);
    if (nbytes < 0) {
        perror("recv");
    } else if (nbytes == 0) {
        // if closed remove it from reactor
        reactor->removeFdFromReactor(fd);
    } else {
        // if has data send it to client handler, dup std in and out to client
        // so every message from him goes to function and from function to him
        buf[nbytes] = '\0';
        string command(buf);
        int res = dup2(fd, STDIN_FILENO);
        int res1 = dup2(fd, STDOUT_FILENO);
        if (res == -1 || res1 == -1) {
            perror("dup2");
            close(fd);
        }
        kosaraju::handle_client_command(reactor->adj, command);
    }
    return nullptr;
}

void *Reactor::handle_connection(Reactor *reactor, int fd)  {
    // get connection and add to poll list
    struct sockaddr_in client_address;
    socklen_t clientAddressLen = sizeof(client_address);
    memset(&client_address, 0, sizeof(client_address));
    clientAddressLen = sizeof(client_address);
    int client_fd = accept(fd, (struct sockaddr *) &client_address, &clientAddressLen);
    if (client_fd == -1) {
        perror("accept");
    } else {
        reactor->addFdToReactor(client_fd, handle_client);
    }
}