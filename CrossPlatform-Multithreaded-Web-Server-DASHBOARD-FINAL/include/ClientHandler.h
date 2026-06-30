#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "SocketUtils.h"
#include "PriorityControl.h"

class ClientHandler {
public:
    static void handle(SocketType clientSocket, bool isPriority, PriorityControl& priorityControl, int workerId);
};

#endif
