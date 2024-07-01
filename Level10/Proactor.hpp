#include <map>
#include <poll.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <pthread.h>
#include <signal.h>

using namespace std;

typedef void * (* proactorFunc) (int sockfd);

// Struct to hold the original function and its argument
typedef struct {
    proactorFunc func;
    int sockfd;
} ThreadData;

// Wrapper function
void* thread_wrapper(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    void* result = data->func(data->sockfd);
    free(data);  // Free the ThreadData after use
    return result;
}

// starts new proactor and returns proactor thread id.
pthread_t startProactor(int sockfd, proactorFunc threadFunc) {
    pthread_t thread;
    ThreadData *data = (ThreadData *) malloc(sizeof(ThreadData));
    if (data == NULL) {
        perror("malloc");
        return (pthread_t) 0;  // Return an invalid thread ID on error
    }
    data->func = threadFunc;
    data->sockfd = sockfd;

    int ret = pthread_create(&thread, NULL, thread_wrapper, data);
    if (ret != 0) {
        perror("pthread_create");
        free(data);
        return (pthread_t) 0;  // Return an invalid thread ID on error
    }

    return thread;
}


// stops proactor by threadid
int stopProactor(pthread_t tid){
    pthread_kill(tid,0);
    return 0;
}