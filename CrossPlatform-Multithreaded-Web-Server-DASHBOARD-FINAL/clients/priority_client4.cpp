#include "client_common.h"
int main(int argc, char* argv[]) {
    std::string serverIp = argc > 1 ? argv[1] : "127.0.0.1";
    sendClientRequest(serverIp, "/pause", "Inder", "Priority");
    return 0;
}
