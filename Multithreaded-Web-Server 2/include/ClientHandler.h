#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "PriorityControl.h"

class ClientHandler {
public:
    static void handle(
        int clientSocket,
        bool isPriority,
        PriorityControl& priorityControl,
        int workerId
    );
};

#endif


