#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include "SocketUtils.h"
#include "RequestQueue.h"
#include "ThreadPool.h"
#include "PriorityControl.h"

class Server {
private:
    SocketType serverSocket;
    int port;
    int workerCount;
    bool multithreadMode;
    std::atomic<bool> running;
    int maxTotalRequests;
    int maxQueueSize;
    std::atomic<int> acceptedRequests;

    RequestQueue requestQueue;
    PriorityControl priorityControl;
    ThreadPool threadPool;

    bool isPriorityRequest(SocketType clientSocket);
    void rejectClient(SocketType clientSocket, const std::string& reason);

public:
    Server(int portNumber = 8080, int workers = 4, bool multiMode = true,
           int totalLimit = 10000, int queueLimit = 500);
    ~Server();

    void start();
    void stop();
};

#endif
