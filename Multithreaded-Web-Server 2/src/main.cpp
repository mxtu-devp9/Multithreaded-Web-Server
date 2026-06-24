#include "Server.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <string>

int main(int argc, char* argv[]) {
    bool multithreadMode = true;

    if (argc > 1) {
        std::string mode = argv[1];

        if (mode == "single") {
            multithreadMode = false;
        } else if (mode == "multi") {
            multithreadMode = true;
        }
    }

    std::cout << "=====================================\n";
    std::cout << " Web Server Starting...\n";
    std::cout << "=====================================\n";

    if (multithreadMode) {
        std::cout << "Execution Mode: MULTITHREADED\n";
    } else {
        std::cout << "Execution Mode: SINGLE THREADED\n";
    }

    Server server(8080, 4, multithreadMode);

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
    std::cout << "Run examples:\n";
    std::cout << "./server single   -> Single-threaded mode\n";
    std::cout << "./server multi     -> Multithreaded mode\n\n";

    server.start();

    return 0;
}