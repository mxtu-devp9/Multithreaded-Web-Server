#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cout << "Client 3: Server connection failed.\n";
        return 1;
    }

    std::cout << "\n=====================================\n";
    std::cout << " CLIENT 3 [ABHI] CONNECTED\n";
    std::cout << "=====================================\n";
    std::cout << "Sending file request...\n";

    std::string request =
        "GET /file HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Client-Name: Abhi\r\n"
        "Client-Type: Normal\r\n"
        "\r\n";

    send(sock, request.c_str(), request.size(), 0);

    char buffer[4096] = {0};
    recv(sock, buffer, sizeof(buffer), 0);

    std::cout << "Server processed Client 3 request.\n";
    std::cout << "Request completed successfully.\n";
    std::cout << "=====================================\n\n";

    close(sock);
    return 0;
}