#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <mutex>

#include "Proactor.hpp"
#include "Reactor.hpp"
#include "kosaraju.hpp"

// The Graph that the Reactor holds
vector<list<int>> adj;
pthread_mutex_t my_mutex;
vector<pthread_t> thread = {};

int get_listener_socket(void) {
    int listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listeningSocket == -1) {
        perror("socket");
        exit(1);
    }

    int enableReuse = 1;
    int ret = setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int));
    if (ret < 0) {
        perror("setsockopt() failed with error code");
        exit(1);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    int bindResult = ::bind(listeningSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (bindResult == -1) {
        perror("Bind failed with error code");
        close(listeningSocket);
        exit(1);
    }

    int listenResult = listen(listeningSocket, 10);
    if (listenResult == -1) {
        printf("listen() failed with error code");
        close(listeningSocket);
        exit(1);
    }

    return listeningSocket;
}

void* handle_client(int fd) {
    while (1) {
        char buf[256];
        cout << "On Thread: Watiting to recv from fd: " << fd << endl;
        int nbytes = recv(fd, buf, sizeof buf, 0);
        if (nbytes < 0) {
            perror("recv");
            cout << "Exit Theard: Fd " << fd << endl;
            break;
        } else if (nbytes == 0) {
            cout << "Exit Theard: Fd " << fd << endl;
            break;
        } else {
            // if has data send it to client handler, dup std in and out to client
            // so every message from him goes to function and from function to him
            buf[nbytes] = '\0';
            string command(buf);
            pthread_mutex_lock(&my_mutex);  // Lock the mutex before accessing counter
            int res = dup2(fd, STDIN_FILENO);
            if (res == -1) {
                perror("dup2");
                close(fd);
            }
            cout << "On Thread: " << fd << " Locked: Mutex Locked" << endl;
            kosaraju::handle_client_command(adj, command);
            pthread_mutex_unlock(&my_mutex);  // Unlock the mutex after access
            cout << "On Thread: " << fd << " UnLocked: Mutex Unlocked" << endl;
        }
    }
    return nullptr;
}

/**
 * Function for listener
 * @param fd
 * @return return pointer to created fd, if error occurred throw exception
 */
void* handle_connection(int fd) {
    // get connection and add to poll list
    struct sockaddr_in client_address;
    socklen_t clientAddressLen = sizeof(client_address);
    memset(&client_address, 0, sizeof(client_address));
    clientAddressLen = sizeof(client_address);
    int client_fd = accept(fd, (struct sockaddr*)&client_address, &clientAddressLen);
    if (client_fd == -1) {
        perror("accept");
        return new int(-1);
    }
    cout << "Client connected, Fd: " << client_fd << endl;
    pthread_t id = startProactor(client_fd, handle_client);
    thread.push_back(id);
    cout << "Create new Thread for Fd: " << client_fd << endl;
    return new int(client_fd);
}

int main() {


    pthread_mutex_init(&my_mutex, NULL);  // Initialize the mutex

    int listener = get_listener_socket();
    if (listener == -1) {
        perror("socket");
        exit(1);
    }

    Reactor* reactor = startReactor();
    addFdToReactor(reactor, listener, handle_connection);
    while (reactor->run) {
        int poll_count = poll(reactor->pfds, reactor->fd_count, -1);
        if (poll_count == -1) {
            perror("poll");
        }

        for (int i = 0; i < reactor->fd_count; i++) {
            if (reactor->pfds[i].revents & POLLIN && reactor->pfds[i].fd != -1) {
                int hot_fd = reactor->pfds[i].fd;
                cout << "The Hot fd is: " << hot_fd << endl;
                if (listener == hot_fd) {
                    void* fds = reactor->f2f[i].func(hot_fd);
                    int res = *(int*)fds;
                    free(fds);
                    if (res == -1) {
                        continue;
                    } else {
                        //                        addFdToReactor(reactor,res, handle_client);
                    }
                }
                //                else{
                //                    void* ret = reactor->f2f[i].func(hot_fd);
                //                    if(*(int*)ret == -1) {
                //                        removeFdFromReactor(reactor, hot_fd);
                //                    }
                //
                //                }
            }
        }
    }
}
