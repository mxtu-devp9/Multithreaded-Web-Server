# Multithreaded Web Server using C++

This project is a C++ multithreaded web server that handles multiple client requests concurrently using a thread pool and shared request queue. It demonstrates important operating system concepts such as threads, mutex, condition variables, producer-consumer model, priority scheduling, race condition prevention, and deadlock avoidance.

## Project Features

- Multithreaded server using C++ threads
- Thread pool for concurrent request handling
- Shared request queue
- Normal client request processing
- Priority client request handling
- Pause and resume mechanism for normal clients
- Mutex and condition variable synchronization
- Real-time browser dashboard
- Live request counter
- Connected devices count
- Thread activity monitor
- Queue status visualization
- Live event logs
- Response time tracking

## Project Working

Multiple clients can send requests to the server from the same system or from different systems connected to the same network. The server accepts incoming requests and places them into a shared request queue. Worker threads from the thread pool pick requests from the queue and process them concurrently.

A special priority client can send a `/pause` request. When this happens, normal clients are temporarily paused. The server processes the priority request first and then resumes the normal clients using a condition variable.

## OS Concepts Used

- Threads
- Thread Pool
- Mutex
- Condition Variable
- Request Queue
- Producer-Consumer Model
- Priority Scheduling
- Race Condition Prevention
- Deadlock Avoidance
- Concurrent Request Processing

## Project Structure
```text
Multithreaded-Web-Server 2
├── 📁 clients
│   ├── ⚡ client1.cpp
│   ├── ⚡ client2.cpp
│   ├── ⚡ client3.cpp
│   ├── ⚡ client_common.h
│   └── ⚡ priority_client4.cpp
├── 📁 include
│   ├── ⚡ ClientHandler.h
│   ├── ⚡ Logger.h
│   ├── ⚡ PriorityControl.h
│   ├── ⚡ RequestQueue.h
│   ├── ⚡ Server.h
│   └── ⚡ ThreadPool.h
├── 📁 logs
├── 📁 src
│   ├── ⚡ ClientHandler.cpp
│   ├── ⚡ Logger.cpp
│   ├── ⚡ PriorityControl.cpp
│   ├── ⚡ RequestQueue.cpp
│   ├── ⚡ Server.cpp
│   ├── ⚡ ThreadPool.cpp
│   └── ⚡ main.cpp
├── 📁 www
│   ├── 🌐 data.html
│   ├── 🌐 index.html
│   └── 🌐 priority.html
├── 📄 Makefile
├── 📝 README.md
├── 📄 client1
├── 📄 client2
├── 📄 client3
├── 📄 priority_client4
└── 📄 server
```
