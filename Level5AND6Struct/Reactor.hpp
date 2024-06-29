#include <map>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "kosaraju.hpp"
#define PORT 9034

using namespace std;

// The Graph that the Reactor holds
vector<list<int>> adj;

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

    int bindResult = ::bind(listeningSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
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
    int client_fd = accept(fd, (struct sockaddr *) &client_address, &clientAddressLen);
    if (client_fd == -1) {
        perror("accept");
    }
    cout <<"client connected"<<endl;
    return new int(client_fd);
}

/**
 * Function to handle clients
 * @param fd
 * @return nothing, if disconnected or error occurred throw exception
 */
void* handle_client(int fd) {
    cout << "Handling client" << endl;
    char buf[256];
    // receive messge from fd
    int nbytes = recv(fd, buf, sizeof buf, 0);
    if (nbytes < 0) {
        perror("recv");
        throw exception();
    }
    else if (nbytes == 0) {
        cout <<"client disconnected"<<endl;
        throw exception();
    }
    else {
        // if has data send it to client handler, dup std in and out to client
        // so every message from him goes to function and from function to him
        buf[nbytes] = '\0';
        string command(buf);
        int res = dup2(fd, STDIN_FILENO);
//        int res1 = dup2(fd, STDOUT_FILENO);
        if (res == -1) {
            perror("dup2");
            close(fd);
        }
        kosaraju::handle_client_command(adj, command);
    }
}

// Define function for clients or listener
typedef void * (*reactorFunc) (int fd);

// map fds to its function
struct fd_to_func{
    int fd;
    reactorFunc func;
};

// Reactor struct
typedef struct Reactor{
    // Poll list
    struct pollfd *pfds;
    // Poll to func
    struct fd_to_func *f2f;
    // Current fds in pfds
    int fd_count;
    // Size of max fds to insert
    int fd_size;
    int run;
    // run poll loop
    int listener;

}Reactor;

/**
 * initialize struct, create listener add to fd to func and poll, return the Reactor
 * @return
 */
Reactor* startReactor() {
    Reactor* r = (Reactor*) malloc(sizeof (Reactor));
    if(!r){
        perror("malloc");
        exit(1);
    }
    r->listener = get_listener_socket();
    if (r->listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }
    // initialize pfds to 5 spots
    r->fd_count = 0;
    r->fd_size = 5;
    r->run = 1;
    // create
    r->pfds = (struct pollfd *) malloc(sizeof *(r->pfds) * r->fd_size);
    if (!r->pfds) {
        perror("poll");
        free(r);
        exit(1);
    }
    r->f2f = (struct fd_to_func *) malloc(sizeof *(r->f2f) * r->fd_size);
    if (!r->f2f) {
        perror("poll");
        free(r);
        exit(1);
    }
    r->pfds[0].fd = r->listener;
    r->pfds[0].events = POLLIN;
    r->f2f[0].fd = r->listener;
    r->f2f[0].func = handle_connection;
    r->fd_count++;
    return r;
}

/**
 * Stops the reactor from running
 * @param r
 * @return
 */
int stopReactor(Reactor* r)  {
    // stop the run
    // free the poll list
    r->run = 0;
    return 0;
}

/**
 * add Fd to reactor
 * @param r - reactor
 * @param fd - file descriptor - client
 * @param func - desired func
 * @return
 */
int addFdToReactor(Reactor* r,int fd, reactorFunc func) {
    // if no room realloc the list
    if (r->fd_count == r->fd_size) {
        r->fd_size *= 2;
        r->pfds = (struct pollfd *) realloc(r->pfds, sizeof(*r->pfds) * r->fd_size);
        r->f2f = (struct fd_to_func *) realloc(r->f2f, sizeof(*r->f2f) * r->fd_size);
    }
    // put fds in next availabe space and update on map
    r->pfds[r->fd_count].fd = fd;
    r->pfds[r->fd_count].events = POLLIN;
    r->f2f[r->fd_count].fd = fd;
    r->f2f[r->fd_count].func = func;
    r->fd_count++;
    cout << "Client Added" <<endl;
    return 0;
}
/**
 * remove fd from reactor
 * @param r - reactor
 * @param fd - file descriptor of client
 * @return
 */
int removeFdFromReactor(Reactor* r,int fd) {
    // search for fds in poll list and remove from map
    for (int i = 0; i < r->fd_count; i++) {
        if (r->pfds[i].fd == fd) {
            r->pfds[i].fd = -1;
            r->f2f[i].fd = -1;
            close(fd);
            return 0;
        }
    }
    return -1;
}



