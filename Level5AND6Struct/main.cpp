#include "Reactor.hpp"

int main(){

    Reactor* reactor = startReactor();
    while(reactor->run){
        int poll_count = poll(reactor->pfds,reactor->fd_count,-1);
        if(poll_count == -1){
            perror("poll");
        }

        for(int i=0;i<reactor->fd_count;i++){
            if(reactor->pfds[i].revents & POLLIN && reactor->pfds[i].fd!=-1){
                int hot_fd = reactor->pfds[i].fd;
                if(reactor->listener == hot_fd){
                    void* fd;
                    try {
                        fd = reactor->f2f[i].func(hot_fd);
                    }
                    catch (exception){
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
