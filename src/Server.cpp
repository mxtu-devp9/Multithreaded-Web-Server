#include "Server.h"
#include "ClientHandler.h"
#include "Logger.h"
#include "DashboardMetrics.h"

#include <iostream>
#include <cstring>
#include <string>

Server::Server(int portNumber, int workers, bool multiMode, int totalLimit, int queueLimit)
    : serverSocket(INVALID_SOCKET_FD),
      port(portNumber),
      workerCount(workers),
      multithreadMode(multiMode),
      running(false),
      maxTotalRequests(totalLimit),
      maxQueueSize(queueLimit),
      acceptedRequests(0),
      threadPool(workers, requestQueue, priorityControl) {
    dashboardSetMaxThreads(workers);
    dashboardSetMaxQueue(queueLimit);
    dashboardSetRequestLimit(totalLimit);
    dashboardSetMode(multiMode ? "MULTITHREADED" : "SINGLE THREADED");
}

Server::~Server() { stop(); }

bool Server::isPriorityRequest(SocketType clientSocket) {
    char buffer[4096] = {0};
    int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, MSG_PEEK);
    if (bytes <= 0) return false;

    std::string request(buffer);
    return request.find("/pause") != std::string::npos || request.find("/priority") != std::string::npos;
}

void Server::rejectClient(SocketType clientSocket, const std::string& reason) {
    dashboardAddRejected(reason);
    std::string body = "<h1>Server Protected</h1><p>Request rejected: " + reason + "</p>";
    std::string response =
        "HTTP/1.1 503 Service Unavailable\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n\r\n" + body;
    send(clientSocket, response.c_str(), (int)response.size(), 0);
    closeSocket(clientSocket);
}

void Server::start() {
    if (!initSockets()) {
        std::cerr << "Socket initialization failed.\n";
        return;
    }

    running = true;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET_FD) {
        std::cerr << "Socket creation failed.\n";
        cleanupSockets();
        return;
    }

    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Bind failed. Port may already be in use.\n";
        closeSocket(serverSocket);
        cleanupSockets();
        return;
    }

    if (listen(serverSocket, maxQueueSize) < 0) {
        std::cerr << "Listen failed.\n";
        closeSocket(serverSocket);
        cleanupSockets();
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

    std::cout << "Max hardware threads used: " << workerCount << "\n";
    std::cout << "Max total accepted requests: " << maxTotalRequests << "\n";
    std::cout << "Max queue size: " << maxQueueSize << "\n";
    std::cout << "Dashboard: http://localhost:" << port << "\n";
    std::cout << "Normal URL: http://localhost:" << port << "/normal\n";
    std::cout << "Priority URL: http://localhost:" << port << "/priority\n";
    std::cout << "Waiting for clients...\n\n";

    while (running) {
        sockaddr_in clientAddress{};
        socklen_t clientSize = sizeof(clientAddress);
        SocketType clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientSize);

        if (clientSocket == INVALID_SOCKET_FD) {
            if (running) std::cerr << "Client accept failed.\n";
            continue;
        }

        int currentAccepted = ++acceptedRequests;
        if (currentAccepted > maxTotalRequests) {
            dashboardSetServerStatus("LIMIT REACHED");
            rejectClient(clientSocket, "Total request limit reached");
            continue;
        }

        int qSize = requestQueue.size();
        dashboardSetQueueSize(qSize);
        if (multithreadMode && qSize >= maxQueueSize) {
            dashboardSetServerStatus("OVERLOAD / QUEUE FULL");
            rejectClient(clientSocket, "Queue limit reached");
            continue;
        }

        bool priority = isPriorityRequest(clientSocket);

        if (multithreadMode) {
            requestQueue.push({clientSocket, priority});
            dashboardSetQueueSize(requestQueue.size());
            if (requestQueue.size() > (maxQueueSize * 8 / 10)) dashboardSetServerStatus("OVERLOAD");
            else if (requestQueue.size() > (maxQueueSize * 4 / 10)) dashboardSetServerStatus("HIGH LOAD");
            else dashboardSetServerStatus("NORMAL");
            std::cout << "[MULTI] Request pushed into queue. Queue: " << requestQueue.size() << "\n";
        } else {
            std::cout << "[SINGLE] Processing request on main thread.\n";
            ClientHandler::handle(clientSocket, priority, priorityControl, 1);
        }
    }
}

void Server::stop() {
    if (!running && serverSocket == INVALID_SOCKET_FD) return;

    running = false;

    if (multithreadMode) {
        requestQueue.shutdown();
        threadPool.stop();
    }

    if (serverSocket != INVALID_SOCKET_FD) {
        closeSocket(serverSocket);
        serverSocket = INVALID_SOCKET_FD;
    }

    cleanupSockets();
    Logger::log("Server stopped.");
}
