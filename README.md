# 🚀 Cross-Platform Multithreaded Web Server using C++

A high-performance **Cross-Platform Multithreaded Web Server** built with **C++17** to demonstrate core **Operating System concepts** including **Thread Pool, Producer–Consumer Model, Priority Scheduling, Mutex, Condition Variables, Request Queue Management, and Overload Protection**.

Supports **macOS** and **Windows** with a **real-time interactive dashboard** for monitoring server activity.

---

## ✨ Features

- 🌐 Cross-Platform (macOS & Windows)
- ⚡ Single & Multithreaded Execution
- 🧵 Dynamic Thread Pool (Hardware Concurrency)
- 📦 Producer–Consumer Request Queue
- 🚦 Priority Scheduling (Pause/Resume)
- 📊 Real-Time Dashboard
- 🔥 Stress Testing & Auto Demo
- 🛡 Queue & Overload Protection
- 📋 Live Event Logging

---

# 🖥 Dashboard

The dashboard displays real-time information including:

- Total, Normal & Priority Requests
- Active & Maximum Threads
- Queue Status
- Connected Devices
- Priority Status
- Thread Activity
- Response Time
- Live Event Logs

---

# ⚙️ Build & Run

## 🍎 macOS

```bash
make clean
make
./server
```

## 🪟 Windows

```cmd
mingw32-make -f Makefile.windows clean
mingw32-make -f Makefile.windows

server.exe
```

Select one of the available modes:

```text
1 → Normal Mode
2 → Stress Mode
3 → Auto Demo / Auto Stress Mode
```

---

# 🧠 Operating System Concepts

- Thread Pool
- Producer–Consumer Model
- Request Queue
- Mutex
- Condition Variable
- Priority Scheduling
- Hardware Concurrency
- Concurrent Processing
- Overload Protection

---

# 🏗 Architecture

```text
Clients
   │
   ▼
Request Queue
   │
   ▼
Thread Pool
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

# 📂 Project Structure

# CrossPlatform-Multithreaded-Web-Server-DASHBOARD-FINAL
```
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
```
---

# 💻 Technologies

- C++17
- Socket Programming
- Multithreading
- POSIX Threads
- Winsock
- HTML
- CSS
- JavaScript

---

# 👨‍💻 Author

**Miten Mistry**  
Master's in Software Engineering  
University of Europe for Applied Sciences
