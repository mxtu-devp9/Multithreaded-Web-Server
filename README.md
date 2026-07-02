# Cross-Platform Multithreaded Web Server using C++

This is a C++ web server that runs on **macOS, Linux, and Windows**. It demonstrates OS concepts such as thread pool, request queue, producer-consumer model, mutex, condition variable, priority scheduling, single-threaded baseline, multithreaded execution, continuous client simulation, and a real-time browser dashboard.

## Features

- Cross-platform socket support using `SocketUtils.h`
- Works on macOS/Linux with POSIX sockets
- Works on Windows with Winsock2
- Single-threaded mode for baseline comparison
- Multithreaded mode using 4 worker threads
- Shared normal and priority request queues
- Priority client support using `/pause` or `/priority`
- Pause/resume mechanism for normal clients
- Real-time dashboard on `http://localhost:8080`
- Automatic demo client that sends 3 requests together
- Continuous request simulation until `CTRL + C`
- Shell and batch scripts for automatic repeated requests
- Multi-device support on the same WiFi network

## Project Structure

```text
CrossPlatform-Multithreaded-Web-Server/
├── include/
│   ├── SocketUtils.h
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
│   ├── client_common.h
│   ├── client1.cpp
│   ├── client2.cpp
│   ├── client3.cpp
│   ├── priority_client4.cpp
│   ├── auto_demo_clients.cpp
│   └── continuous_clients.cpp
├── Makefile
├── Makefile.windows
├── build_windows.bat
├── auto_requests.sh
├── auto_requests.bat
└── README.md
```

## How It Works

1. The server starts on port `8080`.
2. Clients send HTTP requests to the server.
3. The server accepts each connection using TCP sockets.
4. Requests are classified as normal or priority.
5. In single-threaded mode, requests are handled one by one on the main thread.
6. In multithreaded mode, requests are pushed into a shared queue.
7. Worker threads process requests concurrently.
8. Priority requests pause normal clients, execute first, and then resume normal clients.
9. The dashboard updates every few milliseconds using `/status` polling.

## Request Types

Normal requests:

```text
/data
/client-status
/file
/normal
```

Priority requests:

```text
/pause
/priority
```

## macOS / Linux Run Steps

### Compile

```bash
make clean
make
```

### Run single-threaded mode

```bash
./server single
```

### Run multithreaded mode

```bash
./server multi
```

### Open dashboard

```text
http://localhost:8080
```

### Run individual clients

```bash
./client1
./client2
./client3
./priority_client4
```

### Run automatic demo

```bash
./auto_demo_clients
```

### Run continuous client simulation

```bash
./continuous_clients
```

Stop with:

```text
CTRL + C
```

### Run shell auto request script

```bash
./auto_requests.sh
```

## Windows Run Steps

Install **MinGW-w64** and make sure `g++` is available in your terminal.

### Option 1: Build using batch file

```bat
build_windows.bat
```

### Option 2: Build using MinGW make

```bat
mingw32-make -f Makefile.windows clean
mingw32-make -f Makefile.windows
```

### Run single-threaded mode

```bat
server.exe single
```

### Run multithreaded mode

```bat
server.exe multi
```

### Open dashboard

```text
http://localhost:8080
```

### Run individual clients

```bat
client1.exe
client2.exe
client3.exe
priority_client4.exe
```

### Run automatic demo

```bat
auto_demo_clients.exe
```

### Run continuous client simulation

```bat
continuous_clients.exe
```

### Run batch auto request script

```bat
auto_requests.bat
```

Stop with:

```text
CTRL + C
```

## Single vs Multithreaded Comparison

### Single-threaded mode

```bash
./server single
./auto_demo_clients
```

In this mode, the server handles requests one by one.

```text
Client 1
↓
Client 2
↓
Client 3
```

### Multithreaded mode

```bash
./server multi
./auto_demo_clients
```

In this mode, requests are handled concurrently.

```text
Client 1 → Thread 1
Client 2 → Thread 2
Client 3 → Thread 3
```

## Continuous Demo

Run the server:

```bash
./server multi
```

Run continuous clients:

```bash
./continuous_clients
```

This keeps sending requests again and again until you press `CTRL + C`.

## Same WiFi Multi-Device Demo

Find server IP.

macOS:

```bash
ipconfig getifaddr en0
```

Windows:

```bat
ipconfig
```

Open from phone or another laptop:

```text
http://SERVER_IP:8080
```

Normal request:

```text
http://SERVER_IP:8080/normal
```

Priority request:

```text
http://SERVER_IP:8080/priority
```

## OS Concepts Used

- Thread Pool
- Producer-Consumer Model
- Request Queue
- Mutex
- Condition Variable
- Priority Scheduling
- Race Condition Prevention
- Deadlock Avoidance
- Sequential Baseline
- Multithreaded Comparison
- Socket Programming
- Real-Time Monitoring

## Short Explanation

This project implements a cross-platform C++ web server. In single-threaded mode, requests are processed sequentially as a baseline. In multithreaded mode, requests are added to a shared queue and processed by multiple worker threads. A priority client can pause normal clients using condition variables, get processed first, and then resume normal processing. The real-time dashboard displays request counts, thread activity, queue status, response time, and live event logs.
