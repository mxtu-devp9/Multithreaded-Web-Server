#include "client_common.h"
#include <thread>
#include <atomic>
#include <csignal>

std::atomic<bool> running(true);

void signalHandler(int) {
    running = false;
    std::cout << "\nStopping continuous clients...\n";
}

void normalLoop(const std::string& ip, const std::string& path, const std::string& name, int delayMs) {
    while (running) {
        sendClientRequest(ip, path, name, "Normal");
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
}

void priorityLoop(const std::string& ip, int delaySeconds) {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(delaySeconds));
        if (running) sendClientRequest(ip, "/pause", "Inder", "Priority");
    }
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, signalHandler);
    std::string serverIp = argc > 1 ? argv[1] : "127.0.0.1";

    std::cout << "========================================\n";
    std::cout << " CONTINUOUS CLIENTS STARTED\n";
    std::cout << "========================================\n";
    std::cout << "Server IP: " << serverIp << "\n";
    std::cout << "Press CTRL + C to stop.\n\n";

    std::thread c1(normalLoop, serverIp, "/data", "Rishabh", 2000);
    std::thread c2(normalLoop, serverIp, "/client-status", "Avi", 2500);
    std::thread c3(normalLoop, serverIp, "/file", "Abhi", 3000);
    std::thread p(priorityLoop, serverIp, 10);

    c1.join(); c2.join(); c3.join(); p.join();
    std::cout << "Continuous demo stopped.\n";
    return 0;
}
