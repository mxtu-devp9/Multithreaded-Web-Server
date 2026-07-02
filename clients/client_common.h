#ifndef CLIENT_COMMON_H
#define CLIENT_COMMON_H

#include <iostream>
#include <string>
#include <chrono>
#include "../include/SocketUtils.h"

inline void sendClientRequest(const std::string& serverIp, const std::string& path, const std::string& name, const std::string& type) {
    if (!initSockets()) {
        std::cout << name << ": socket initialization failed.\n";
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    SocketType sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET_FD) {
        std::cout << name << ": socket creation failed.\n";
        cleanupSockets();
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);

    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cout << name << ": invalid server IP.\n";
        closeSocket(sock);
        cleanupSockets();
        return;
    }

    if (connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cout << name << ": connection failed.\n";
        closeSocket(sock);
        cleanupSockets();
        return;
    }

    std::string request =
        "GET " + path + "?name=" + name + "&type=" + type + " HTTP/1.1\r\n"
        "Host: " + serverIp + "\r\n"
        "Client-Name: " + name + "\r\n"
        "Client-Type: " + type + "\r\n"
        "Connection: close\r\n"
        "\r\n";

    send(sock, request.c_str(), (int)request.size(), 0);

    char buffer[4096] = {0};
    recv(sock, buffer, sizeof(buffer) - 1, 0);

    auto end = std::chrono::high_resolution_clock::now();
    auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "[DONE] " << name << " -> " << path << " completed in " << timeMs << " ms\n";

    closeSocket(sock);
    cleanupSockets();
}

#endif
