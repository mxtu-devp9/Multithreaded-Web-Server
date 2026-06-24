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
        std::cout << "Priority Client 4: Server connection failed.\n";
        return 1;
    }

    std::cout << "\n=====================================\n";
    std::cout << " PRIORITY CLIENT 4 CONNECTED\n";
    std::cout << "=====================================\n";
    std::cout << "Sending /pause request to server...\n";
    std::cout << "Normal clients will pause temporarily.\n\n";

    std::string request =
        "GET /pause HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Client-Name: Inder\r\n"
        "Client-Type: Priority\r\n"
        "\r\n";

    send(sock, request.c_str(), request.size(), 0);

    char buffer[4096];
    recv(sock, buffer, sizeof(buffer), 0);

    std::cout << "Priority request processed successfully.\n";
    std::cout << "Normal clients resumed automatically.\n";
    std::cout << "=====================================\n\n";

    close(sock);
    return 0;
}