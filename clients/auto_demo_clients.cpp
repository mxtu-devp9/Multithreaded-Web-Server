#include "client_common.h"
#include <thread>
#include <vector>

int main(int argc, char* argv[]) {
    std::string serverIp = argc > 1 ? argv[1] : "127.0.0.1";
    int rounds = argc > 2 ? std::stoi(argv[2]) : 1;

    std::cout << "========================================\n";
    std::cout << " AUTO DEMO: 3 CLIENTS TOGETHER\n";
    std::cout << "========================================\n";
    std::cout << "Server IP: " << serverIp << " | Rounds: " << rounds << "\n\n";

    for (int i = 1; i <= rounds; i++) {
        std::cout << "------------- ROUND " << i << " -------------\n";
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::thread> clients;
        clients.emplace_back(sendClientRequest, serverIp, "/data", "Rishabh", "Normal");
        clients.emplace_back(sendClientRequest, serverIp, "/client-status", "Avi", "Normal");
        clients.emplace_back(sendClientRequest, serverIp, "/file", "Abhi", "Normal");

        for (auto& t : clients) if (t.joinable()) t.join();

        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Round " << i << " total time: " << ms << " ms\n\n";
    }

    std::cout << "Sending priority request...\n";
    sendClientRequest(serverIp, "/pause", "Inder", "Priority");
    return 0;
}
