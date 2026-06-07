#include "ClientHandler.h"
#include "Logger.h"

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <unistd.h>
#include <sys/socket.h>

std::mutex dashboardMutex;

int totalRequests = 0;
int normalRequests = 0;
int priorityRequests = 0;
int connectedDevices = 0;

std::string lastClient = "None";
std::string lastType = "None";
std::string lastThread = "None";
std::string priorityStatus = "WAITING";
std::string normalStatus = "ACTIVE";
std::string responseTime = "0 ms";

std::string threadStatus[5] = {
    "",
    "IDLE",
    "IDLE",
    "IDLE",
    "IDLE"
};

std::vector<std::string> eventLog;

std::string nowTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm = std::localtime(&t);

    std::stringstream ss;
    ss << std::put_time(tm, "%H:%M:%S");
    return ss.str();
}

void addEvent(const std::string& msg) {
    eventLog.push_back(nowTime() + " - " + msg);

    if (eventLog.size() > 8) {
        eventLog.erase(eventLog.begin());
    }
}

std::string getHeaderValue(const std::string& request, const std::string& key) {
    size_t pos = request.find(key + ": ");
    if (pos == std::string::npos) return "Browser";

    pos += key.length() + 2;
    size_t end = request.find("\r\n", pos);

    return request.substr(pos, end - pos);
}

std::string getPath(const std::string& request) {
    size_t start = request.find("GET ");
    if (start == std::string::npos) return "/";
    start += 4;

    size_t end = request.find(" ", start);
    if (end == std::string::npos) return "/";

    return request.substr(start, end - start);
}

std::string getQueryValue(const std::string& path, const std::string& key) {
    std::string pattern = key + "=";
    size_t pos = path.find(pattern);

    if (pos == std::string::npos) return "";

    pos += pattern.length();
    size_t end = path.find("&", pos);

    if (end == std::string::npos) {
        return path.substr(pos);
    }

    return path.substr(pos, end - pos);
}

std::string getStatusJson() {
    std::lock_guard<std::mutex> lock(dashboardMutex);

    std::string events = "";
    for (size_t i = 0; i < eventLog.size(); i++) {
        events += "<div class='log'>" + eventLog[i] + "</div>";
    }

    return "{"
        "\"total\":\"" + std::to_string(totalRequests) + "\","
        "\"normalReq\":\"" + std::to_string(normalRequests) + "\","
        "\"priorityReq\":\"" + std::to_string(priorityRequests) + "\","
        "\"devices\":\"" + std::to_string(connectedDevices) + "\","
        "\"client\":\"" + lastClient + "\","
        "\"type\":\"" + lastType + "\","
        "\"thread\":\"" + lastThread + "\","
        "\"priority\":\"" + priorityStatus + "\","
        "\"normal\":\"" + normalStatus + "\","
        "\"response\":\"" + responseTime + "\","
        "\"t1\":\"" + threadStatus[1] + "\","
        "\"t2\":\"" + threadStatus[2] + "\","
        "\"t3\":\"" + threadStatus[3] + "\","
        "\"t4\":\"" + threadStatus[4] + "\","
        "\"events\":\"" + events + "\""
        "}";
}

std::string dashboardHtml() {
    return
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n\r\n"
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>Live Multithreaded Server Dashboard</title>"
        "<style>"
        "body{margin:0;font-family:Arial;background:#0f172a;color:white;}"
        ".container{padding:35px;}"
        ".title{font-size:38px;font-weight:bold;color:#38bdf8;}"
        ".subtitle{color:#cbd5e1;margin-top:8px;font-size:17px;}"
        ".grid{display:grid;grid-template-columns:repeat(4,1fr);gap:18px;margin-top:25px;}"
        ".grid2{display:grid;grid-template-columns:repeat(3,1fr);gap:18px;margin-top:18px;}"
        ".card{background:#1e293b;border-radius:16px;padding:22px;border:1px solid #334155;box-shadow:0 10px 25px rgba(0,0,0,.25);}"
        ".big{font-size:30px;font-weight:bold;color:#22c55e;}"
        ".ok{color:#22c55e;font-weight:bold;}"
        ".warn{color:#facc15;font-weight:bold;}"
        ".danger{color:#ef4444;font-weight:bold;}"
        ".box{background:#020617;padding:12px;border-radius:10px;margin-top:8px;}"
        ".thread{padding:10px;background:#020617;border-radius:10px;margin:8px 0;}"
        ".log{background:#020617;margin:8px 0;padding:10px;border-radius:8px;color:#cbd5e1;}"
        ".footer{margin-top:25px;color:#94a3b8;}"
        "</style>"
        "</head>"
        "<body>"
        "<div class='container'>"
        "<div class='title'>Live Multithreaded Web Server Dashboard</div>"
        "<div class='subtitle'>C++ Thread Pool + Request Queue + Priority Pause/Resume + Real-time Monitoring</div>"

        "<div class='grid'>"
        "<div class='card'><h3>Total Requests</h3><div id='total' class='big'>0</div></div>"
        "<div class='card'><h3>Normal Requests</h3><div id='normalReq' class='big'>0</div></div>"
        "<div class='card'><h3>Priority Requests</h3><div id='priorityReq' class='big'>0</div></div>"
        "<div class='card'><h3>Connected Devices</h3><div id='devices' class='big'>0</div></div>"
        "</div>"

        "<div class='grid2'>"
        "<div class='card'><h2>Latest Client</h2><p id='client' class='ok'>None</p><p>Type: <span id='type'>None</span></p><p>Thread: <span id='thread'>None</span></p><p>Response Time: <span id='response'>0 ms</span></p></div>"
        "<div class='card'><h2>Priority Status</h2><p id='priority' class='warn'>WAITING</p><h2>Normal Clients</h2><p id='normal' class='ok'>ACTIVE</p></div>"
        "<div class='card'><h2>Queue Visualization</h2><div class='box'>Normal Queue: <span id='normalQueue'>ACTIVE</span></div><div class='box'>Priority Queue: <span id='priorityQueue'>WAITING</span></div></div>"
        "</div>"

        "<div class='grid2'>"
        "<div class='card'><h2>Thread Monitor</h2><div class='thread'>Thread-1: <span id='t1'>IDLE</span></div><div class='thread'>Thread-2: <span id='t2'>IDLE</span></div><div class='thread'>Thread-3: <span id='t3'>IDLE</span></div><div class='thread'>Thread-4: <span id='t4'>IDLE</span></div></div>"
        "<div class='card'><h2>OS Concepts</h2><p>Producer-Consumer</p><p>Thread Pool</p><p>Mutex</p><p>Condition Variable</p><p>Priority Scheduling</p></div>"
        "<div class='card'><h2>Live Event Log</h2><div id='events'></div></div>"
        "</div>"

        "<div class='footer'>Open from other systems: http://SERVER_IP:8080/data?name=Rishabh&type=Normal</div>"
        "</div>"

        "<script>"
        "async function updateDashboard(){"
        "let res=await fetch('/status');"
        "let d=await res.json();"
        "document.getElementById('total').innerText=d.total;"
        "document.getElementById('normalReq').innerText=d.normalReq;"
        "document.getElementById('priorityReq').innerText=d.priorityReq;"
        "document.getElementById('devices').innerText=d.devices;"
        "document.getElementById('client').innerText=d.client;"
        "document.getElementById('type').innerText=d.type;"
        "document.getElementById('thread').innerText=d.thread;"
        "document.getElementById('priority').innerText=d.priority;"
        "document.getElementById('normal').innerText=d.normal;"
        "document.getElementById('response').innerText=d.response;"
        "document.getElementById('normalQueue').innerText=d.normal;"
        "document.getElementById('priorityQueue').innerText=d.priority;"
        "document.getElementById('t1').innerText=d.t1;"
        "document.getElementById('t2').innerText=d.t2;"
        "document.getElementById('t3').innerText=d.t3;"
        "document.getElementById('t4').innerText=d.t4;"
        "document.getElementById('events').innerHTML=d.events;"
        "document.getElementById('priority').className=d.priority.includes('PROCESSING')?'danger':'warn';"
        "document.getElementById('normal').className=d.normal.includes('PAUSED')?'danger':'ok';"
        "}"
        "setInterval(updateDashboard,700);"
        "updateDashboard();"
        "</script>"
        "</body>"
        "</html>";
}

void ClientHandler::handle(
    int clientSocket,
    bool isPriority,
    PriorityControl& priorityControl,
    int workerId
) {
    auto start = std::chrono::high_resolution_clock::now();

    char buffer[4096] = {0};
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0) {
        close(clientSocket);
        return;
    }

    std::string request(buffer);
    std::string path = getPath(request);

    if (path.find("/status") == 0) {
        std::string json = getStatusJson();
        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n\r\n" + json;

        send(clientSocket, response.c_str(), response.size(), 0);
        close(clientSocket);
        return;
    }

    if (path == "/" || path.find("/index") == 0) {
        std::string response = dashboardHtml();
        send(clientSocket, response.c_str(), response.size(), 0);
        close(clientSocket);
        return;
    }

    std::string clientName = getQueryValue(path, "name");
    std::string clientType = getQueryValue(path, "type");

    if (clientName == "") clientName = getHeaderValue(request, "Client-Name");
    if (clientType == "") clientType = getHeaderValue(request, "Client-Type");

    std::string threadName = "Thread-" + std::to_string(workerId);

    {
        std::lock_guard<std::mutex> lock(dashboardMutex);
        totalRequests++;
        connectedDevices++;
        lastClient = clientName;
        lastType = clientType;
        lastThread = threadName;
        threadStatus[workerId] = "ACTIVE";
        addEvent(clientName + " connected and assigned to " + threadName);
    }

    if (isPriority || path.find("/pause") == 0) {
        {
            std::lock_guard<std::mutex> lock(dashboardMutex);
            priorityRequests++;
            priorityStatus = "PROCESSING PRIORITY";
            normalStatus = "PAUSED";
            threadStatus[workerId] = "PROCESSING PRIORITY";
            addEvent("Priority client " + clientName + " requested /pause");
            addEvent("Normal clients paused");
        }

        priorityControl.pauseNormalClients();
        std::this_thread::sleep_for(std::chrono::seconds(4));
        priorityControl.resumeNormalClients();

        {
            std::lock_guard<std::mutex> lock(dashboardMutex);
            priorityStatus = "COMPLETED";
            normalStatus = "ACTIVE / RESUMED";
            threadStatus[workerId] = "COMPLETED";
            addEvent("Priority request completed");
            addEvent("Normal clients resumed");
        }

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<h1>Priority Request Completed</h1>";

        send(clientSocket, response.c_str(), response.size(), 0);
    } else {
        {
            std::lock_guard<std::mutex> lock(dashboardMutex);
            normalRequests++;
            priorityStatus = "WAITING";
            normalStatus = "ACTIVE";
            addEvent("Normal request processing: " + clientName);
        }

        priorityControl.waitIfPaused();
        std::this_thread::sleep_for(std::chrono::seconds(2));

        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        {
            std::lock_guard<std::mutex> lock(dashboardMutex);
            responseTime = std::to_string(ms) + " ms";
            threadStatus[workerId] = "COMPLETED";
            addEvent("Request completed for " + clientName);
        }

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n\r\n"
            "<h1>Normal Request Completed</h1>";

        send(clientSocket, response.c_str(), response.size(), 0);
    }

    {
        std::lock_guard<std::mutex> lock(dashboardMutex);
        threadStatus[workerId] = "IDLE";
    }

    Logger::log("Request processed for: " + clientName);
    close(clientSocket);
}