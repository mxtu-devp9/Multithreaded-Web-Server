#include "Server.h"
#include "Logger.h"

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(int portNumber, int workerCount)
    : port(portNumber), serverSocket(-1), running(false),
      threadPool(workerCount, requestQueue, priorityControl) {}

Server::~Server() {
    stop();
}

bool Server::isPriorityRequest(int clientSocket) {
    // Look at request without removing it from socket permanently.
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));

    int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, MSG_PEEK);
    if (bytes <= 0) return false;

    std::string request(buffer);
    return request.find("GET /pause") != std::string::npos ||
           request.find("GET /priority") != std::string::npos;
}

void Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        throw std::runtime_error("Failed to create server socket.");
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        throw std::runtime_error("Failed to bind server socket. Port may already be in use.");
    }

    if (listen(serverSocket, 10) < 0) {
        throw std::runtime_error("Failed to listen on server socket.");
    }

    running = true;
    threadPool.start();

    Logger::log("Server started on port " + std::to_string(port));
    Logger::log("Normal clients: /, /data.html");
    Logger::log("Priority client: /pause or /priority");

    while (running) {
        sockaddr_in clientAddress{};
        socklen_t clientLength = sizeof(clientAddress);

        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientLength);
        if (clientSocket < 0) {
            if (running) {
                Logger::log("Failed to accept client.");
            }
            continue;
        }

        bool priority = isPriorityRequest(clientSocket);

        if (priority) {
            Logger::log("Accepted PRIORITY client request.");
        } else {
            Logger::log("Accepted normal client request.");
        }

        requestQueue.push({clientSocket, priority});
    }
}

void Server::stop() {
    if (!running && serverSocket == -1) return;

    running = false;

    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }

    threadPool.stop();
    Logger::log("Server stopped.");
}
