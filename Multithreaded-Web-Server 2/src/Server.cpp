#include "Server.h"
#include "ClientHandler.h"
#include "Logger.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

Server::Server(int portNumber, int workers, bool multiMode)
    : serverSocket(-1),
      port(portNumber),
      workerCount(workers),
      multithreadMode(multiMode),
      running(false),
      threadPool(workers, requestQueue, priorityControl) {}

Server::~Server() {
    stop();
}

bool Server::isPriorityRequest(int clientSocket) {
    char buffer[4096] = {0};

    int bytes = recv(clientSocket, buffer, sizeof(buffer), MSG_PEEK);

    if (bytes <= 0) {
        return false;
    }

    std::string request(buffer);

    if (request.find("/pause") != std::string::npos ||
        request.find("/priority") != std::string::npos) {
        return true;
    }

    return false;
}

void Server::start() {
    running = true;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) {
        std::cerr << "Socket creation failed.\n";
        return;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Bind failed. Port may already be in use.\n";
        return;
    }

    if (listen(serverSocket, 20) < 0) {
        std::cerr << "Listen failed.\n";
        return;
    }

    if (multithreadMode) {
        threadPool.start();
        Logger::log("Server started in MULTITHREADED mode on port " + std::to_string(port));
        std::cout << "Server started in MULTITHREADED mode.\n";
    } else {
        Logger::log("Server started in SINGLE THREADED mode on port " + std::to_string(port));
        std::cout << "Server started in SINGLE THREADED mode.\n";
    }

    std::cout << "Waiting for clients...\n\n";

    while (running) {
        sockaddr_in clientAddress{};
        socklen_t clientSize = sizeof(clientAddress);

        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientSize);

        if (clientSocket < 0) {
            if (running) {
                std::cerr << "Client accept failed.\n";
            }
            continue;
        }

        bool priority = isPriorityRequest(clientSocket);

        if (multithreadMode) {
            ClientRequest request;
            request.clientSocket = clientSocket;
            request.isPriority = priority;

            requestQueue.push(request);

            std::cout << "[MULTI] Request pushed into queue.\n";
        } else {
            std::cout << "[SINGLE] Processing request on main thread.\n";

            ClientHandler::handle(
                clientSocket,
                priority,
                priorityControl,
                1
            );
        }
    }
}

void Server::stop() {
    if (!running && serverSocket == -1) {
        return;
    }

    running = false;

    if (multithreadMode) {
        requestQueue.shutdown();
        threadPool.stop();
    }

    if (serverSocket != -1) {
        close(serverSocket);
        serverSocket = -1;
    }

    Logger::log("Server stopped.");
}