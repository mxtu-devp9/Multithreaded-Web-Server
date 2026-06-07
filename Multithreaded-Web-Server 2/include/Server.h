#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include "RequestQueue.h"
#include "ThreadPool.h"
#include "PriorityControl.h"

class Server {
private:
    int serverSocket;
    int port;
    std::atomic<bool> running;

    RequestQueue requestQueue;
    PriorityControl priorityControl;
    ThreadPool threadPool;

    bool isPriorityRequest(int clientSocket);

public:
    Server(int portNumber = 8080, int workerCount = 4);
    ~Server();

    void start();
    void stop();
};

#endif