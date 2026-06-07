CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -Iinclude

SERVER_SRC = src/main.cpp src/Server.cpp src/ThreadPool.cpp src/RequestQueue.cpp src/ClientHandler.cpp src/Logger.cpp src/PriorityControl.cpp

all: server client1 client2 client3 priority_client4

server: $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $(SERVER_SRC) -o server

client1: clients/client1.cpp
	$(CXX) $(CXXFLAGS) clients/client1.cpp -o client1

client2: clients/client2.cpp
	$(CXX) $(CXXFLAGS) clients/client2.cpp -o client2

client3: clients/client3.cpp
	$(CXX) $(CXXFLAGS) clients/client3.cpp -o client3

priority_client4: clients/priority_client4.cpp
	$(CXX) $(CXXFLAGS) clients/priority_client4.cpp -o priority_client4

clean:
	rm -f server client1 client2 client3 priority_client4
