#include "Logger.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
std::mutex Logger::logMutex;

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);

    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);

    std::tm tmNow{};
#ifdef _WIN32
    localtime_s(&tmNow, &timeNow);
#else
    localtime_r(&timeNow, &tmNow);
#endif

    std::ostringstream line;
    line << "[" << std::put_time(&tmNow, "%H:%M:%S") << "] " << message;

    std::cout << line.str() << std::endl;

    std::ofstream file("logs/server.log", std::ios::app);
    if (file.is_open()) file << line.str() << std::endl;
}
