#include "RequestQueue.h"

RequestQueue::RequestQueue() : shutdownFlag(false) {}

void RequestQueue::push(const ClientRequest& request) {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (request.isPriority) priorityQueue.push(request);
    else normalQueue.push(request);
    queueCondition.notify_one();
}

ClientRequest RequestQueue::pop() {
    std::unique_lock<std::mutex> lock(queueMutex);

    queueCondition.wait(lock, [this]() {
        return shutdownFlag || !priorityQueue.empty() || !normalQueue.empty();
    });

    if (shutdownFlag) return {INVALID_SOCKET_FD, false};

    if (!priorityQueue.empty()) {
        ClientRequest request = priorityQueue.front();
        priorityQueue.pop();
        return request;
    }

    ClientRequest request = normalQueue.front();
    normalQueue.pop();
    return request;
}

int RequestQueue::size() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return static_cast<int>(normalQueue.size() + priorityQueue.size());
}

int RequestQueue::normalSize() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return static_cast<int>(normalQueue.size());
}

int RequestQueue::prioritySize() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return static_cast<int>(priorityQueue.size());
}

void RequestQueue::shutdown() {
    std::lock_guard<std::mutex> lock(queueMutex);
    shutdownFlag = true;
    queueCondition.notify_all();
}
