#include <poll.h>
#include <unistd.h>
#include <iostream>
#define PORT 9034

using namespace std;

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

}Reactor;

/**
 * initialize struct, create listener add to fd to func and poll, return the Reactor
 * @return
 */
Reactor* startReactor() ;

/**
 * Stops the reactor from running
 * @param r
 * @return
 */
int stopReactor(Reactor* r);

/**
 * add Fd to reactor
 * @param r - reactor
 * @param fd - file descriptor - client
 * @param func - desired func
 * @return
 */
int addFdToReactor(Reactor* r,int fd, reactorFunc func);
/**
 * remove fd from reactor
 * @param r - reactor
 * @param fd - file descriptor of client
 * @return
 */
int removeFdFromReactor(Reactor* r,int fd);



