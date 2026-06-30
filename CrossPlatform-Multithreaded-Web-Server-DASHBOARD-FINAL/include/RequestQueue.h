#ifndef REQUEST_QUEUE_H
#define REQUEST_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include "SocketUtils.h"

struct ClientRequest {
    SocketType clientSocket;
    bool isPriority;
};

class RequestQueue {
private:
    std::queue<ClientRequest> normalQueue;
    std::queue<ClientRequest> priorityQueue;
    mutable std::mutex queueMutex;
    std::condition_variable queueCondition;
    bool shutdownFlag;

public:
    RequestQueue();
    void push(const ClientRequest& request);
    ClientRequest pop();
    int size() const;
    int normalSize() const;
    int prioritySize() const;
    void shutdown();
};

#endif
