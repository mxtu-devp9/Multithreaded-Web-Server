#include "ThreadPool.h"
#include "ClientHandler.h"
#include "Logger.h"

ThreadPool::ThreadPool(int count, RequestQueue& queue, PriorityControl& control)
    : threadCount(count), requestQueue(queue), priorityControl(control), running(false) {}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::start() {
    running = true;

    for (int i = 0; i < threadCount; i++) {
        workers.emplace_back(&ThreadPool::workerLoop, this, i + 1);
    }

    Logger::log("Thread pool started with " + std::to_string(threadCount) + " worker threads.");
}

void ThreadPool::stop() {
    if (!running) return;

    running = false;
    requestQueue.shutdown();

    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    Logger::log("Thread pool stopped.");
}

void ThreadPool::workerLoop(int workerId) {
    while (running) {
        ClientRequest request = requestQueue.pop();

        if (request.clientSocket == -1) {
            break;
        }

        ClientHandler::handle(request.clientSocket, request.isPriority, priorityControl, workerId);
    }
}
