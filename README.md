# 🚀 Cross-Platform Multithreaded Web Server using C++

A high-performance **Cross-Platform Multithreaded Web Server** built using **C++17**, designed to demonstrate core **Operating System concepts** such as **Thread Pools, Producer–Consumer Architecture, Priority Scheduling, Condition Variables, Mutex Synchronization, Request Queue Management, and Overload Protection**.

The project supports both **macOS** and **Windows** and includes a **real-time interactive dashboard** for monitoring server activity, worker threads, queue status, and live request processing.

---

## ✨ Features

- 🌐 Cross-Platform (macOS & Windows)
- ⚡ Single Threaded & Multithreaded Modes
- 🧵 Dynamic Thread Pool (Hardware Concurrency)
- 📦 Producer–Consumer Request Queue
- 🚦 Priority Scheduling
- ⏸ Pause / Resume Normal Clients
- 🔥 Automatic Stress Testing
- 🤖 Auto Demo Mode
- 📊 Real-Time Dashboard
- 📈 Live Request Monitoring
- 🖥 Thread Activity Monitoring
- 📋 Live Event Logging
- 🛡 Overload Protection
- 🚀 Queue Limit Protection
- 📡 Cross-Platform Socket Programming

---

# 🖥 Dashboard

The integrated dashboard provides real-time monitoring of the server.

### Live Metrics

- Total Requests
- Normal Requests
- Priority Requests
- Connected Devices
- Queue Size
- Queue Limit
- Maximum Threads
- Active Threads
- Response Time
- Thread Status
- Priority Status
- Normal Client Status
- Server Status
- Live Event Log

Dashboard updates automatically without refreshing the browser.

---

# ⚙️ Build & Run

This project supports both **macOS** and **Windows**.

---

## 🍎 macOS

### Clean Previous Build

```bash
make clean
```

### Compile

```bash
make
```

### Run Server

```bash
./server
```

Choose one of the following modes:

```text
1 → Normal Mode

2 → Stress Mode

3 → Auto Demo / Auto Stress Mode
```

### Run Individual Clients

```bash
./client1
./client2
./client3
./priority_client4
```

### Run Automatic Demo

```bash
./auto_demo_clients
```

### Run Continuous Client

```bash
./continuous_clients
```

### Run Stress Client

```bash
./stress_client
```

Stop the server anytime using:

```text
CTRL + C
```

---

# 🪟 Windows

This project can be built using **MinGW/MSYS2** or **Visual Studio**.

### Build using Makefile.windows

```cmd
mingw32-make -f Makefile.windows clean
mingw32-make -f Makefile.windows
```

Or simply run:

```cmd
build_windows.bat
```

### Run Server

```cmd
server.exe
```

Choose one of the following modes:

```text
1 → Normal Mode

2 → Stress Mode

3 → Auto Demo / Auto Stress Mode
```

### Run Individual Clients

```cmd
client1.exe
client2.exe
client3.exe
priority_client4.exe
```

### Run Automatic Demo

```cmd
auto_demo_clients.exe
```

### Run Continuous Client

```cmd
continuous_clients.exe
```

### Run Stress Client

```cmd
stress_client.exe
```

Stop the server anytime using:

```text
CTRL + C
```

# 👥 Client Endpoints

| Client | Endpoint |
|---------|----------|
| Client 1 | `/data` |
| Client 2 | `/client-status` |
| Client 3 | `/file` |
| Priority Client | `/pause` |

---

# 🚦 Priority Scheduling

Whenever a priority request arrives:

```text
Priority Request
        │
        ▼
Pause Normal Clients
        │
        ▼
Execute Priority Request
        │
        ▼
Resume Normal Clients
```

During execution:

- Normal clients are paused.
- Priority request gets immediate execution.
- Dashboard reflects the status in real time.

---

# 🔥 Stress Mode

Stress Mode automatically generates a large number of concurrent client requests to test server performance.

It demonstrates:

- High request throughput
- Thread Pool utilization
- Queue Management
- Overload Protection
- Concurrent Processing

---

# 📊 Thread Processing

## Single Threaded

```text
Client 1
   │
   ▼
Client 2
   │
   ▼
Client 3
```

Only one request is processed at a time.

---

## Multithreaded

```text
Client 1 ──► Thread 1

Client 2 ──► Thread 2

Client 3 ──► Thread 3

Priority ─► Highest Priority
```

Multiple requests execute simultaneously, reducing response time and increasing throughput.

---

# 🏗 Server Architecture

```text
              Clients
                 │
                 ▼
          Incoming Requests
                 │
                 ▼
          Shared Request Queue
                 │
        ┌────────┴────────┐
        ▼                 ▼
   Thread Pool      Priority Queue
        │
        ▼
 Worker Threads
        │
        ▼
 Priority Scheduling
        │
        ▼
 HTTP Response
        │
        ▼
 Live Dashboard
```

---

# 🧠 Operating System Concepts Used

| Concept | Purpose |
|----------|---------|
| Thread Pool | Efficient thread reuse |
| Producer–Consumer | Queue based request handling |
| Request Queue | Stores incoming requests |
| Mutex | Prevents race conditions |
| Condition Variable | Pause / Resume synchronization |
| Priority Scheduling | Executes priority requests first |
| Hardware Concurrency | Uses maximum available CPU threads |
| Concurrent Processing | Parallel request execution |
| Queue Synchronization | Safe shared queue access |
| Overload Protection | Prevents server crashes |

---

# 🛡 Overload Protection

The server automatically protects itself by:

- Limiting maximum requests
- Limiting queue size
- Rejecting excessive connections
- Preventing crashes under heavy load
- Displaying overload status on the dashboard

---

# 📂 Project Structure

# CrossPlatform-Multithreaded-Web-Server-DASHBOARD-FINAL

├── 📁 clients
│   ├── ⚡ auto_demo_clients.cpp
│   ├── ⚡ client1.cpp
│   ├── ⚡ client2.cpp
│   ├── ⚡ client3.cpp
│   ├── ⚡ client_common.h
│   ├── ⚡ continuous_clients.cpp
│   ├── ⚡ priority_client4.cpp
│   └── ⚡ stress_client.cpp
├── 📁 docs
├── 📁 include
│   ├── ⚡ ClientHandler.h
│   ├── ⚡ DashboardMetrics.h
│   ├── ⚡ Logger.h
│   ├── ⚡ PriorityControl.h
│   ├── ⚡ RequestQueue.h
│   ├── ⚡ Server.h
│   ├── ⚡ SocketUtils.h
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
├── 📄 Makefile.windows
├── 📝 README.md
├── 📄 auto_demo_clients
├── 📄 auto_requests.bat
├── 📄 auto_requests.sh
├── 📄 build_windows.bat
├── 📄 client1
├── 📄 client2
├── 📄 client3
├── 📄 continuous_clients
├── 📄 priority_client4
├── 📄 server
└── 📄 stress_client


---

# 💻 Technologies

- C++17
- Multithreading
- POSIX Threads
- Winsock
- HTML
- CSS
- JavaScript
- Socket Programming
- Makefile

---

# 📈 Learning Outcomes

This project demonstrates practical implementation of:

- Multithreaded Programming
- Operating System Concepts
- Synchronization Techniques
- Concurrent Server Design
- Queue Management
- Thread Scheduling
- Cross-Platform Development
- Network Programming
- Real-Time Monitoring
- Performance Optimization

---

# 🚀 Future Improvements

- WebSocket-based real-time updates
- HTTPS support
- Authentication
- REST API
- Dynamic thread scaling
- Load balancing
- Distributed server using MPI
- Docker deployment

---

# 👨‍💻 Author

## **Miten Mistry**
