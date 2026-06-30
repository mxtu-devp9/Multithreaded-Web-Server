#include "PriorityControl.h"

PriorityControl::PriorityControl() : paused(false) {}

void PriorityControl::pauseNormalClients() {
    std::lock_guard<std::mutex> lock(pauseMutex);
    paused = true;
}

void PriorityControl::resumeNormalClients() {
    {
        std::lock_guard<std::mutex> lock(pauseMutex);
        paused = false;
    }
    pauseCondition.notify_all();
}

void PriorityControl::waitIfPaused() {
    std::unique_lock<std::mutex> lock(pauseMutex);
    pauseCondition.wait(lock, [this]() { return !paused; });
}
