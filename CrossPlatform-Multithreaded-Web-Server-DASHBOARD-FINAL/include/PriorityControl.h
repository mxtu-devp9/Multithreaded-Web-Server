#ifndef PRIORITY_CONTROL_H
#define PRIORITY_CONTROL_H

#include <mutex>
#include <condition_variable>

class PriorityControl {
private:
    bool paused;
    std::mutex pauseMutex;
    std::condition_variable pauseCondition;

public:
    PriorityControl();
    void pauseNormalClients();
    void resumeNormalClients();
    void waitIfPaused();
};

#endif
