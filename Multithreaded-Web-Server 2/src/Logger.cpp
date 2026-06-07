#include "Logger.h"

#include <ctime>
#include <fstream>
#include <iostream>

std::mutex Logger::logMutex;

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);

    std::time_t now = std::time(nullptr);
    char timeBuffer[100];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    std::string finalMessage = std::string("[") + timeBuffer + "] " + message;

    std::cout << finalMessage << std::endl;

    std::ofstream file("logs/server.log", std::ios::app);
    if (file.is_open()) {
        file << finalMessage << std::endl;
    }
}
