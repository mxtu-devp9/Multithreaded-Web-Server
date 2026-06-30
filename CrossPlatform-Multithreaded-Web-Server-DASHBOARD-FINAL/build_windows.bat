@echo off
echo Building cross-platform multithreaded web server for Windows...
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Server.cpp src/ThreadPool.cpp src/RequestQueue.cpp src/ClientHandler.cpp src/Logger.cpp src/PriorityControl.cpp -o server.exe -lws2_32
g++ -std=c++17 -Wall -Wextra -Iinclude clients/client1.cpp -o client1.exe -lws2_32
g++ -std=c++17 -Wall -Wextra -Iinclude clients/client2.cpp -o client2.exe -lws2_32
g++ -std=c++17 -Wall -Wextra -Iinclude clients/client3.cpp -o client3.exe -lws2_32
g++ -std=c++17 -Wall -Wextra -Iinclude clients/priority_client4.cpp -o priority_client4.exe -lws2_32
g++ -std=c++17 -Wall -Wextra -Iinclude clients/auto_demo_clients.cpp -o auto_demo_clients.exe -lws2_32
g++ -std=c++17 -Wall -Wextra -Iinclude clients/continuous_clients.cpp -o continuous_clients.exe -lws2_32
echo Build completed.
pause
