#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using SocketType = SOCKET;
    const SocketType INVALID_SOCKET_FD = INVALID_SOCKET;
    inline void closeSocket(SocketType socketFd) { closesocket(socketFd); }
    inline bool initSockets() {
        WSADATA wsaData;
        return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
    }
    inline void cleanupSockets() { WSACleanup(); }
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    using SocketType = int;
    const SocketType INVALID_SOCKET_FD = -1;
    inline void closeSocket(SocketType socketFd) { close(socketFd); }
    inline bool initSockets() { return true; }
    inline void cleanupSockets() {}
#endif

#endif
