#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <unistd.h>
#include <arpa/inet.h>

void sendRequest(
    const std::string& serverIp,
    const std::string& path,
    const std::string& name,
    const std::string& type
) {
    auto start = std::chrono::high_resolution_clock::now();

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        std::cout << name << " socket creation failed.\n";
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);

    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cout << name << " invalid server IP.\n";
        close(sock);
        return;
    }

    if (connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cout << name << " connection failed.\n";
        close(sock);
        return;
    }

    std::string request =
        "GET " + path + "?name=" + name + "&type=" + type + " HTTP/1.1\r\n"
        "Host: " + serverIp + "\r\n"
        "Client-Name: " + name + "\r\n"
        "Client-Type: " + type + "\r\n"
        "\r\n";

    std::cout << "[SEND] " << name << " -> " << path << "\n";

    send(sock, request.c_str(), request.size(), 0);

    char buffer[4096] = {0};
    recv(sock, buffer, sizeof(buffer), 0);

    auto end = std::chrono::high_resolution_clock::now();
    auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "[DONE] " << name << " completed in " << timeMs << " ms\n";

    close(sock);
}

int main(int argc, char* argv[]) {
    std::string serverIp = "127.0.0.1";
    int rounds = 1;

    if (argc > 1) {
        serverIp = argv[1];
    }

    if (argc > 2) {
        rounds = std::stoi(argv[2]);
    }

    std::cout << "========================================\n";
    std::cout << " AUTO CLIENT DEMO STARTED\n";
    std::cout << "========================================\n";
    std::cout << "Server IP : " << serverIp << "\n";
    std::cout << "Rounds    : " << rounds << "\n\n";

    auto demoStart = std::chrono::high_resolution_clock::now();

    for (int i = 1; i <= rounds; i++) {
        std::cout << "\n------------- ROUND " << i << " -------------\n";

        auto roundStart = std::chrono::high_resolution_clock::now();

        std::vector<std::thread> clients;

        clients.emplace_back(sendRequest, serverIp, "/data", "Rishabh", "Normal");
        clients.emplace_back(sendRequest, serverIp, "/client-status", "Avi", "Normal");
        clients.emplace_back(sendRequest, serverIp, "/file", "Abhi", "Normal");

        for (auto& client : clients) {
            if (client.joinable()) {
                client.join();
            }
        }

        auto roundEnd = std::chrono::high_resolution_clock::now();
        auto roundTime = std::chrono::duration_cast<std::chrono::milliseconds>(roundEnd - roundStart).count();

        std::cout << "Round " << i << " total time: " << roundTime << " ms\n";

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    std::cout << "\n------------- PRIORITY TEST -------------\n";
    sendRequest(serverIp, "/pause", "Inder", "Priority");

    auto demoEnd = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(demoEnd - demoStart).count();

    std::cout << "\n========================================\n";
    std::cout << " AUTO DEMO COMPLETED\n";
    std::cout << "Total execution time: " << totalTime << " ms\n";
    std::cout << "========================================\n";

    return 0;
}