#include "RequestQueue.h"

RequestQueue::RequestQueue() : shuttingDown(false) {}

void RequestQueue::push(const ClientRequest& request) {
    std::lock_guard<std::mutex> lock(queueMutex);

    if (request.isPriority) {
        priorityQueue.push(request);
    } else {
        normalQueue.push(request);
    }

    queueCondition.notify_one();
}

ClientRequest RequestQueue::pop() {
    std::unique_lock<std::mutex> lock(queueMutex);

    queueCondition.wait(lock, [this]() {
        return shuttingDown || !priorityQueue.empty() || !normalQueue.empty();
    });

    if (shuttingDown) {
        return {-1, false};
    }

    // Priority queue is checked first.
    if (!priorityQueue.empty()) {
        ClientRequest request = priorityQueue.front();
        priorityQueue.pop();
        return request;
    }

    ClientRequest request = normalQueue.front();
    normalQueue.pop();
    return request;
}

void RequestQueue::shutdown() {
    std::lock_guard<std::mutex> lock(queueMutex);
    shuttingDown = true;
    queueCondition.notify_all();
}
