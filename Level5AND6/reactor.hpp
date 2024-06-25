#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <vector>
#include <functional>

class Reactor {
public:
    Reactor();
    ~Reactor();

    bool start();
    bool stop();
    bool addFd(int fd, short events);
    bool removeFd(int fd);
    void setHandler(std::function<void(int)> handler);
    void handleEvents();

private:
    struct EventData {
        int fd;
        short events;
    };

    int epoll_fd;
    std::function<void(int)> event_handler;
    std::vector<EventData> event_data;
};

#endif // REACTOR_HPP
