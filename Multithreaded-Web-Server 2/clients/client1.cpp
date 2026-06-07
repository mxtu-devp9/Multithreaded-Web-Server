#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cout << "Client 1: Server connection failed.\n";
        return 1;
    }

    std::cout << "Client 1 [Rishabh] connected.\n";
    std::cout << "Sending normal request...\n";

    std::string request =
        "GET /data HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Client-Name: Rishabh\r\n"
        "Client-Type: Normal\r\n"
        "\r\n";

    send(sock, request.c_str(), request.size(), 0);

    char buffer[4096];
    recv(sock, buffer, sizeof(buffer), 0);

    std::cout << "Client 1 response received successfully.\n";

    close(sock);
    return 0;
}