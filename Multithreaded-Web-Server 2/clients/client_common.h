#ifndef CLIENT_COMMON_H
#define CLIENT_COMMON_H

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

inline void sendRequest(const std::string& clientName, const std::string& path) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << clientName << ": socket creation failed\n";
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << clientName << ": connection failed. Start server first.\n";
        close(sock);
        return;
    }

    std::string request = "GET " + path + " HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    send(sock, request.c_str(), request.size(), 0);

    char buffer[4096];
    std::memset(buffer, 0, sizeof(buffer));
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);

    std::cout << "\n===== " << clientName << " Response =====\n";
    if (bytes > 0) {
        std::cout << buffer << std::endl;
    } else {
        std::cout << "No response received.\n";
    }

    close(sock);
}

#endif
