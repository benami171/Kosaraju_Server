#include "Reactor.hpp"
#include "kosaraju.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


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
void* handle_connection (int fd) {
    // get connection and add to poll list
    struct sockaddr_in client_address;
    socklen_t clientAddressLen = sizeof(client_address);
    memset(&client_address, 0, sizeof(client_address));
    clientAddressLen = sizeof(client_address);
    int client_fd = accept(fd, (struct sockaddr *) &client_address, &clientAddressLen);
    if (client_fd == -1) {
        perror("accept");
        throw exception();
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


int main(){

    int listener = get_listener_socket();
    if(listener == -1){
        perror("socket");
        exit(1);
    }

    Reactor* reactor = startReactor();
    addFdToReactor(reactor,listener,handle_connection);
    while(reactor->run){
        int poll_count = poll(reactor->pfds,reactor->fd_count,-1);
        if(poll_count == -1){
            perror("poll");
        }

        for(int i=0;i<reactor->fd_count;i++){
            if(reactor->pfds[i].revents & POLLIN && reactor->pfds[i].fd!=-1){
                int hot_fd = reactor->pfds[i].fd;
                if(listener == hot_fd){
                    void* fd;
                    try {
                        fd = reactor->f2f[i].func(hot_fd);
                    }
                    catch (exception){
                        // need to continue loop
                        continue;
                    }
                    int fds = *(int*)fd;
                    free(fd);
                    addFdToReactor(reactor,fds, handle_client);
                }
                else{
                    try {
                        reactor->f2f[i].func(hot_fd);
                    }
                    catch (exception){
                        removeFdFromReactor(reactor,hot_fd);
                    }
                }
            }
        }
    }
}
