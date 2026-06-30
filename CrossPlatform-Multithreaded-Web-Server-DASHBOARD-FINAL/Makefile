CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -Iinclude

SERVER_SRC = src/main.cpp src/Server.cpp src/ThreadPool.cpp src/RequestQueue.cpp src/ClientHandler.cpp src/Logger.cpp src/PriorityControl.cpp
CLIENTS = client1 client2 client3 priority_client4 auto_demo_clients continuous_clients stress_client

all: server $(CLIENTS)

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

continuous_clients:
	$(CXX) $(CXXFLAGS) clients/continuous_clients.cpp -o continuous_clients

stress_client:
	$(CXX) $(CXXFLAGS) clients/stress_client.cpp -o stress_client

clean:
	rm -f server client1 client2 client3 priority_client4 auto_demo_clients continuous_clients stress_client *.exe
