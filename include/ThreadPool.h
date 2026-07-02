#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <atomic>
#include "RequestQueue.h"
#include "PriorityControl.h"

class ThreadPool {
private:
    int threadCount;
    std::vector<std::thread> workers;
    RequestQueue& requestQueue;
    PriorityControl& priorityControl;
    std::atomic<bool> running;

    void workerLoop(int workerId);

public:
    ThreadPool(int count, RequestQueue& queue, PriorityControl& control);
    ~ThreadPool();
    void start();
    void stop();
};

#endif
