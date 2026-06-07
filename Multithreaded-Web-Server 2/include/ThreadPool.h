#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "PriorityControl.h"
#include "RequestQueue.h"

#include <atomic>
#include <thread>
#include <vector>

class ThreadPool {
private:
    int threadCount;
    RequestQueue& requestQueue;
    PriorityControl& priorityControl;
    std::vector<std::thread> workers;
    std::atomic<bool> running;

    void workerLoop(int workerId);

public:
    ThreadPool(int count, RequestQueue& queue, PriorityControl& control);
    ~ThreadPool();

    void start();
    void stop();
};

#endif
