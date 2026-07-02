#include "client_common.h"
#include <thread>
#include <vector>
#include <algorithm>

int main(int argc, char* argv[]) {
    std::string serverIp = argc > 1 ? argv[1] : "127.0.0.1";
    int totalRequests = argc > 2 ? std::stoi(argv[2]) : 12000;
    int batchSize = argc > 3 ? std::stoi(argv[3]) : 120;

    std::vector<std::string> paths = {"/data", "/client-status", "/file", "/normal"};

    std::cout << "========================================\n";
    std::cout << " EXCESSIVE CONNECTION / STRESS CLIENT\n";
    std::cout << "========================================\n";
    std::cout << "Server IP: " << serverIp << "\n";
    std::cout << "Total Requests: " << totalRequests << "\n";
    std::cout << "Batch Size: " << batchSize << "\n\n";

    int sent = 0;
    while (sent < totalRequests) {
        std::vector<std::thread> batch;
        int currentBatch = std::min(batchSize, totalRequests - sent);

        for (int i = 0; i < currentBatch; ++i) {
            int id = sent + i + 1;
            bool priority = (id % 250 == 0);
            std::string path = priority ? "/pause" : paths[id % paths.size()];
            std::string type = priority ? "Priority" : "Normal";
            std::string name = priority ? "StressPriority" : "StressClient" + std::to_string(id);
            batch.emplace_back(sendClientRequest, serverIp, path, name, type);
        }

        for (auto& t : batch) if (t.joinable()) t.join();
        sent += currentBatch;
        std::cout << "Sent " << sent << " / " << totalRequests << " requests\n";
    }

    std::cout << "Stress test completed. Check dashboard for rejected requests / overload.\n";
    return 0;
}
