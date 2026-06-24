CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -Iinclude

SERVER_SRC = src/main.cpp src/Server.cpp src/ThreadPool.cpp src/RequestQueue.cpp src/ClientHandler.cpp src/Logger.cpp src/PriorityControl.cpp

all: server client1 client2 client3 priority_client4 auto_demo_clients

server:
	$(CXX) $(CXXFLAGS) $(SERVER_SRC) -o server

client1:
	$(CXX) $(CXXFLAGS) clients/client1.cpp -o client1

client2:
	$(CXX) $(CXXFLAGS) clients/client2.cpp -o client2

client3:
	$(CXX) $(CXXFLAGS) clients/client3.cpp -o client3

priority_client4:
	$(CXX) $(CXXFLAGS) clients/priority_client4.cpp -o priority_client4

auto_demo_clients:
	$(CXX) $(CXXFLAGS) clients/auto_demo_clients.cpp -o auto_demo_clients

clean:
	rm -f server client1 client2 client3 priority_client4 auto_demo_clients