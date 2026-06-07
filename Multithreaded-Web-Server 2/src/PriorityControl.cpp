#include "PriorityControl.h"

PriorityControl::PriorityControl() {
    paused = false;
}

void PriorityControl::pauseNormalClients() {
    std::lock_guard<std::mutex> lock(mtx);
    paused = true;
}

void PriorityControl::resumeNormalClients() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        paused = false;
    }
    cv.notify_all();
}

void PriorityControl::waitIfPaused() {
    std::unique_lock<std::mutex> lock(mtx);

    cv.wait(lock, [this]() {
        return paused == false;
    });
}