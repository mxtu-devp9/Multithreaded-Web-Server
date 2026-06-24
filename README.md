# 🚀 Multithreaded Web Server using C++

## How to Run

### Compile

```bash
make clean
make
```

### Run Single Threaded Server

```bash
./server single
```

### Run Multithreaded Server

```bash
./server multi
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

### Run Continuous Requests

```bash
./continuous_clients
```

Stop anytime using:

```text
CTRL + C
```

---

# How It Works

### Server

The server listens on **Port 8080** and accepts requests from multiple clients.

### Request Queue

Incoming requests are stored in a shared queue before processing.

### Thread Pool

The server creates **4 worker threads** that process requests concurrently.

### Normal Clients

* Client 1 → `/data`
* Client 2 → `/status`
* Client 3 → `/file`

These requests are added to the normal queue.

### Priority Client

Client 4 sends:

```text
/pause
```

When a priority request arrives:

1. Normal clients are paused.
2. Priority request is processed first.
3. Normal clients are resumed.

### Dashboard

The browser dashboard shows:

* Total Requests
* Normal Requests
* Priority Requests
* Connected Devices
* Thread Status
* Queue Status
* Response Time
* Live Event Logs

All values update in real time.

---

# Single vs Multithreaded

### Single Threaded

```bash
./server single
```

Requests are processed one by one.

```text
Client 1
↓
Client 2
↓
Client 3
```

### Multithreaded

```bash
./server multi
```

Requests are processed simultaneously.

```text
Client 1 → Thread 1
Client 2 → Thread 2
Client 3 → Thread 3
```

This reduces response time and increases throughput.

---

# OS Concepts Used

* Thread Pool
* Producer Consumer Model
* Request Queue
* Mutex
* Condition Variable
* Priority Scheduling
* Race Condition Prevention
* Deadlock Avoidance
* Concurrent Processing

---

# File Structure

# File Tree: Multithreaded-Web-Server 2

```
├── 📁 clients
│   ├── ⚡ auto_demo_clients.cpp
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
├── 📄 auto_demo_clients
├── 📄 auto_request.sh
├── 📄 client1
├── 📄 client2
├── 📄 client3
├── 📄 priority_client4
└── 📄 server
```



# Author

**Miten Mistry**
