# Priority-Based Multithreaded Web Server in C++

## Project Idea

This project is a simple C++ multithreaded web server. Three normal clients send requests to the main server. A fourth priority client can send `/pause`. When the server receives this priority request, normal client processing is paused temporarily. The priority client is served first, and then normal clients resume automatically.

## OS Concepts Covered

- Process: Server and clients are separate programs.
- Threads: Worker threads handle multiple clients.
- Thread Pool: Fixed worker threads are created once and reused.
- Mutex: Protects shared queue and log file.
- Condition Variable: Workers wait until a request is available.
- Producer-Consumer: Server accepts clients and workers process them.
- Priority Scheduling: Priority client is served before normal clients.
- Pause/Resume Synchronization: Normal clients wait during priority mode.
- Race Condition Prevention: Shared resources are accessed safely.
- Graceful Shutdown: Ctrl+C stops the server safely.

## Folder Structure

```text
Multithreaded-Web-Server/
├── include/
│   ├── Server.h
│   ├── ThreadPool.h
│   ├── RequestQueue.h
│   ├── ClientHandler.h
│   ├── PriorityControl.h
│   └── Logger.h
├── src/
│   ├── main.cpp
│   ├── Server.cpp
│   ├── ThreadPool.cpp
│   ├── RequestQueue.cpp
│   ├── ClientHandler.cpp
│   ├── PriorityControl.cpp
│   └── Logger.cpp
├── clients/
│   ├── client1.cpp
│   ├── client2.cpp
│   ├── client3.cpp
│   ├── priority_client4.cpp
│   └── client_common.h
├── www/
│   ├── index.html
│   ├── data.html
│   └── priority.html
├── logs/
│   └── server.log
├── Makefile
└── README.md
```

## How to Compile

```bash
make
```

## How to Run

Open one terminal and start the server:

```bash
./server
```

Open three separate terminals and run normal clients:

```bash
./client1
./client2
./client3
```

Then open another terminal and run the priority client:

```bash
./priority_client4
```

## Expected Behaviour

1. Client 1, Client 2, and Client 3 send normal requests.
2. Server handles them using worker threads.
3. Client 4 sends `/pause` request.
4. Server activates priority mode.
5. Normal clients wait temporarily.
6. Client 4 is processed first.
7. Server disables priority mode.
8. Normal clients resume.

## Clean Build Files

```bash
make clean
```
