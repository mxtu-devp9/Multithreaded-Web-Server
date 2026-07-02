#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <mutex>

class Logger {
private:
    static std::mutex logMutex;

public:
    static void log(const std::string& message);
};

#endif
