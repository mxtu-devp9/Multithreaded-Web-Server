#include "Server.h"
#include "SocketUtils.h"
#include "DashboardMetrics.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <string>
#include <vector>
#include <atomic>
#include <algorithm>

void openBrowser() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
#ifdef __APPLE__
    system("open http://localhost:8080");
#elif _WIN32
    system("start http://localhost:8080");
#else
    system("xdg-open http://localhost:8080");
#endif
}

void sendAutoRequest(const std::string& path, const std::string& name, const std::string& type) {
    SocketType sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET_FD) return;

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        closeSocket(sock);
        return;
    }

    std::string request =
        "GET " + path + "?name=" + name + "&type=" + type + " HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Client-Name: " + name + "\r\n"
        "Client-Type: " + type + "\r\n"
        "Connection: close\r\n\r\n";

    send(sock, request.c_str(), (int)request.size(), 0);
    char buffer[512] = {0};
    recv(sock, buffer, sizeof(buffer) - 1, 0);
    closeSocket(sock);
}

void runAutoDemo(int totalRequests, int batchSize) {
    if (!initSockets()) return;

    dashboardSetMode("AUTO DEMO / STRESS MODE");
    dashboardAddEventPublic("Auto Demo started: generating " + std::to_string(totalRequests) + " requests");

    std::vector<std::string> paths = {"/data", "/client-status", "/file", "/normal"};
    int sent = 0;

    while (sent < totalRequests) {
        std::vector<std::thread> batch;
        int currentBatch = std::min(batchSize, totalRequests - sent);

        for (int i = 0; i < currentBatch; ++i) {
            int id = sent + i + 1;
            bool priority = (id % 100 == 0);

            if (priority) {
                dashboardSetPriorityVisual(true);
                dashboardAddEventPublic("AUTO DEMO: Priority arrived - normal clients RED/PAUSED");

                // Run priority request synchronously so the dashboard stays red while priority is processing.
                sendAutoRequest("/pause", "AutoPriority", "Priority");

                dashboardSetPriorityVisual(false);
                dashboardAddEventPublic("AUTO DEMO: Priority finished - normal clients GREEN/RUNNING");
            } else {
                std::string path = paths[id % paths.size()];
                std::string name = "AutoClient" + std::to_string(id);
                batch.emplace_back(sendAutoRequest, path, name, "Normal");
            }
        }

        for (auto& t : batch) {
            if (t.joinable()) t.join();
        }

        sent += currentBatch;

        if (sent > totalRequests * 8 / 10) dashboardSetServerStatus("OVERLOAD");
        else if (sent > totalRequests * 4 / 10) dashboardSetServerStatus("HIGH LOAD");
        else dashboardSetServerStatus("NORMAL");

        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }

    dashboardSetPriorityVisual(false);
    dashboardAddEventPublic("Auto Demo completed");
    cleanupSockets();
}

int main(int argc, char* argv[]) {
    int choice = 0;
    bool multithreadMode = true;

    unsigned int workers = std::thread::hardware_concurrency();
    if (workers == 0) workers = 4;

    int totalLimit = 10000;
    int queueLimit = 500;
    int autoRequests = 12000;
    int batchSize = 120;

    if (argc > 1) {
        std::string mode = argv[1];
        if (mode == "single") choice = 1;
        else if (mode == "multi") choice = 1;
        else if (mode == "stress") choice = 2;
        else if (mode == "auto") choice = 3;
        if (mode == "single") multithreadMode = false;
    }

    if (choice == 0) {
        std::cout << "====================================\n";
        std::cout << " Cross Platform Multithreaded Server\n";
        std::cout << "====================================\n\n";
        std::cout << "1. Normal Mode\n";
        std::cout << "2. Stress Mode\n";
        std::cout << "3. Auto Demo\n\n";
        std::cout << "Select Option: ";
        std::cin >> choice;
    }

    if (choice == 1) {
        dashboardSetMode(multithreadMode ? "MULTITHREADED" : "SINGLE THREADED");
        totalLimit = 10000;
        queueLimit = 500;
    } else if (choice == 2) {
        dashboardSetMode("STRESS MODE");
        totalLimit = 8000;
        queueLimit = 350;
        autoRequests = 9000;
        batchSize = 100;
    } else {
        dashboardSetMode("AUTO DEMO / STRESS MODE");
        totalLimit = 10000;
        queueLimit = 500;
        autoRequests = 12000;
        batchSize = 120;
    }

    std::cout << "\nDetected hardware threads: " << workers << "\n";
    std::cout << "Using worker threads: " << workers << "\n";
    std::cout << "Max total requests before protection: " << totalLimit << "\n";
    std::cout << "Max queue size: " << queueLimit << "\n";

    Server server(8080, static_cast<int>(workers), multithreadMode, totalLimit, queueLimit);

    std::thread browserThread(openBrowser);
    browserThread.detach();

    std::thread serverThread([&server]() {
        server.start();
    });

    if (choice == 2 || choice == 3) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cout << "\nAuto stress generator started...\n";
        std::cout << "Generating " << autoRequests << " requests in batches of " << batchSize << "\n";
        std::thread demoThread(runAutoDemo, autoRequests, batchSize);
        demoThread.detach();
    }

    std::cout << "\nDashboard: http://localhost:8080\n";
    std::cout << "Press CTRL + C to stop server.\n\n";

    if (serverThread.joinable()) serverThread.join();
    return 0;
}
