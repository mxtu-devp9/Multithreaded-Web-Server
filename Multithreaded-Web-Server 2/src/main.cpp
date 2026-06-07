#include "Server.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>

int main() {
    std::cout << "=====================================\n";
    std::cout << " Multithreaded Web Server Starting...\n";
    std::cout << "=====================================\n";

    Server server(8080, 4);

    std::thread browserThread([]() {
        std::this_thread::sleep_for(std::chrono::seconds(2));

#ifdef __APPLE__
        system("open http://localhost:8080");
#elif _WIN32
        system("start http://localhost:8080");
#else
        system("xdg-open http://localhost:8080");
#endif
    });

    browserThread.detach();

    std::cout << "Server running on: http://localhost:8080\n";
    std::cout << "Press CTRL + C to stop server.\n\n";

    server.start();

    return 0;
}