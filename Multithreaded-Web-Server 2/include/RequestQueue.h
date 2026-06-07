#ifndef REQUEST_QUEUE_H
#define REQUEST_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>

// This structure represents one client request accepted by the server.
struct ClientRequest {
    int clientSocket;
    bool isPriority;
};

// Shared request queue used between the main server thread and worker threads.
// OS concept: Producer-Consumer problem using mutex and condition_variable.
class RequestQueue {
private:
    std::queue<ClientRequest> normalQueue;
    std::queue<ClientRequest> priorityQueue;

    std::mutex queueMutex;
    std::condition_variable queueCondition;
    bool shuttingDown;

public:
    RequestQueue();

    void push(const ClientRequest& request);
    ClientRequest pop();
    void shutdown();
};

#endif
