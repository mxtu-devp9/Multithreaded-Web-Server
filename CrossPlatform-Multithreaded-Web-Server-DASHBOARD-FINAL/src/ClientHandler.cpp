#include "ClientHandler.h"
#include "Logger.h"
#include "DashboardMetrics.h"

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <thread>

int totalRequests = 0;
int normalRequests = 0;
int priorityRequests = 0;
int connectedDevices = 0;
int rejectedRequests = 0;
int currentQueueSize = 0;
int maxQueueSize = 500;
int maxThreadCount = 4;
int requestLimit = 10000;

std::string lastClient = "None";
std::string lastType = "None";
std::string lastThread = "None";
std::string priorityStatus = "WAITING";
std::string normalStatus = "ACTIVE";
std::string responseTime = "0 ms";
std::string executionMode = "MULTITHREADED";
std::string serverStatus = "NORMAL";

bool priorityVisualActive = false;

std::vector<std::string> threadStatus(65, "IDLE");
std::vector<std::string> eventLog;

std::mutex dashboardMutex;
std::mutex eventLogMutex;

std::string nowTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tmNow{};

#ifdef _WIN32
    localtime_s(&tmNow, &t);
#else
    localtime_r(&t, &tmNow);
#endif

    std::stringstream ss;
    ss << std::put_time(&tmNow, "%H:%M:%S");
    return ss.str();
}

void addEvent(const std::string& message) {
    std::lock_guard<std::mutex> lock(eventLogMutex);
    eventLog.push_back(nowTime() + " - " + message);

    if (eventLog.size() > 14) {
        eventLog.erase(eventLog.begin());
    }
}

void dashboardAddEventPublic(const std::string& message) {
    addEvent(message);
}

void dashboardSetMode(const std::string& mode) {
    std::lock_guard<std::mutex> lock(dashboardMutex);
    executionMode = mode;
}

void dashboardSetMaxThreads(int count) {
    std::lock_guard<std::mutex> lock(dashboardMutex);
    maxThreadCount = count;

    if ((int)threadStatus.size() <= count + 1) {
        threadStatus.resize(count + 2, "IDLE");
    }
}

void dashboardSetMaxQueue(int count) {
    std::lock_guard<std::mutex> lock(dashboardMutex);
    maxQueueSize = count;
}

void dashboardSetRequestLimit(int count) {
    std::lock_guard<std::mutex> lock(dashboardMutex);
    requestLimit = count;
}

void dashboardSetQueueSize(int count) {
    std::lock_guard<std::mutex> lock(dashboardMutex);
    currentQueueSize = count;
}

void dashboardSetServerStatus(const std::string& status) {
    std::lock_guard<std::mutex> lock(dashboardMutex);
    serverStatus = status;
}

void dashboardSetPriorityVisual(bool active) {
    std::lock_guard<std::mutex> lock(dashboardMutex);
    priorityVisualActive = active;

    if (active) {
        priorityStatus = "RUNNING";
        normalStatus = "PAUSED";
    } else {
        priorityStatus = "WAITING";
        normalStatus = "ACTIVE";
    }
}

void dashboardAddRejected(const std::string& reason) {
    {
        std::lock_guard<std::mutex> lock(dashboardMutex);
        rejectedRequests++;
        serverStatus = "OVERLOAD / PROTECTED";
    }

    addEvent("Rejected request: " + reason);
}

std::string escapeJson(const std::string& s) {
    std::string out;

    for (char c : s) {
        if (c == '"') {
            out += "\\\"";
        } else if (c == '\\') {
            out += "\\\\";
        } else if (c == '\n' || c == '\r') {
            out += " ";
        } else {
            out += c;
        }
    }

    return out;
}

std::string getHeaderValue(const std::string& request, const std::string& key) {
    size_t pos = request.find(key + ": ");

    if (pos == std::string::npos) {
        return "Browser";
    }

    pos += key.length() + 2;
    size_t end = request.find("\r\n", pos);

    if (end == std::string::npos) {
        return "Browser";
    }

    return request.substr(pos, end - pos);
}

std::string getPath(const std::string& request) {
    size_t start = request.find("GET ");

    if (start == std::string::npos) {
        return "/";
    }

    start += 4;
    size_t end = request.find(" ", start);

    if (end == std::string::npos) {
        return "/";
    }

    return request.substr(start, end - start);
}

std::string getQueryValue(const std::string& path, const std::string& key) {
    std::string pattern = key + "=";
    size_t pos = path.find(pattern);

    if (pos == std::string::npos) {
        return "";
    }

    pos += pattern.length();
    size_t end = path.find("&", pos);

    if (end == std::string::npos) {
        return path.substr(pos);
    }

    return path.substr(pos, end - pos);
}

std::string getStatusJson() {
    std::lock_guard<std::mutex> dashboardLock(dashboardMutex);

    std::vector<std::string> eventsCopy;

    {
        std::lock_guard<std::mutex> eventLock(eventLogMutex);
        eventsCopy = eventLog;
    }

    std::string events;

    for (const auto& e : eventsCopy) {
        events += "<div class='log'>" + e + "</div>";
    }

    int activeThreads = 0;
    std::string threadHtml;

    int displayThreads = maxThreadCount;

    if (displayThreads > 24) {
        displayThreads = 24;
    }

    for (int i = 1; i <= displayThreads; ++i) {
        std::string st = "IDLE";

        if (i < (int)threadStatus.size()) {
            st = threadStatus[i];
        }

        bool active = st != "IDLE";

        if (active) {
            activeThreads++;
        }

        threadHtml += "<div class='thread " + std::string(active ? "threadActive" : "threadIdle") + "'>";
        threadHtml += "<span class='threadDot'></span><span>Thread-" + std::to_string(i) + "</span>";
        threadHtml += "<b>" + st + "</b></div>";
    }

    std::string priorityOut = priorityVisualActive ? "RUNNING" : priorityStatus;
    std::string normalOut = priorityVisualActive ? "PAUSED" : normalStatus;

    return "{"
           "\"mode\":\"" + escapeJson(executionMode) + "\","
           "\"serverStatus\":\"" + escapeJson(serverStatus) + "\","
           "\"total\":\"" + std::to_string(totalRequests) + "\","
           "\"normalReq\":\"" + std::to_string(normalRequests) + "\","
           "\"priorityReq\":\"" + std::to_string(priorityRequests) + "\","
           "\"rejected\":\"" + std::to_string(rejectedRequests) + "\","
           "\"queueSize\":\"" + std::to_string(currentQueueSize) + "\","
           "\"maxQueue\":\"" + std::to_string(maxQueueSize) + "\","
           "\"maxThreads\":\"" + std::to_string(maxThreadCount) + "\","
           "\"activeThreads\":\"" + std::to_string(activeThreads) + "\","
           "\"requestLimit\":\"" + std::to_string(requestLimit) + "\","
           "\"devices\":\"" + std::to_string(connectedDevices) + "\","
           "\"client\":\"" + escapeJson(lastClient) + "\","
           "\"type\":\"" + escapeJson(lastType) + "\","
           "\"thread\":\"" + escapeJson(lastThread) + "\","
           "\"priority\":\"" + escapeJson(priorityOut) + "\","
           "\"normal\":\"" + escapeJson(normalOut) + "\","
           "\"response\":\"" + escapeJson(responseTime) + "\","
           "\"threadHtml\":\"" + escapeJson(threadHtml) + "\","
           "\"events\":\"" + escapeJson(events) + "\""
           "}";
}

std::string dashboardHtml() {
    std::string html = R"HTML(HTTP/1.1 200 OK
Content-Type: text/html
Connection: close

<!DOCTYPE html>
<html>
<head>
<title>Live Multithreaded Server Dashboard</title>
<style>
:root{
    --bg:#020617;
    --panel:rgba(15,23,42,.74);
    --panel2:rgba(30,41,59,.78);
    --border:rgba(148,163,184,.22);
    --text:#f8fafc;
    --muted:#94a3b8;
    --green:#22c55e;
    --red:#ef4444;
    --yellow:#facc15;
    --cyan:#38bdf8;
    --blue:#60a5fa;
}
*{box-sizing:border-box;}
body{
    margin:0;
    font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Arial,sans-serif;
    color:var(--text);
    background:
        radial-gradient(circle at 20% 0%,rgba(56,189,248,.18),transparent 30%),
        radial-gradient(circle at 90% 5%,rgba(59,130,246,.14),transparent 28%),
        linear-gradient(135deg,#020617 0%,#07111f 50%,#020617 100%);
}
.container{padding:18px 22px 24px;}
.topbar{
    display:flex;
    align-items:center;
    justify-content:space-between;
    gap:16px;
    margin-bottom:14px;
}
.titleBlock h1{
    margin:0;
    font-size:22px;
    line-height:1.1;
    color:#7dd3fc;
    letter-spacing:.2px;
}
.subtitle{color:#cbd5e1;margin-top:5px;font-size:13px;}
.macDots{display:flex;gap:8px;align-items:center;}
.macDots span{width:12px;height:12px;border-radius:50%;display:block;box-shadow:0 0 16px rgba(255,255,255,.15);}
.macDots span:nth-child(1){background:#ff5f57;}.macDots span:nth-child(2){background:#ffbd2e;}.macDots span:nth-child(3){background:#28c840;}
.modePill{padding:10px 14px;border:1px solid var(--border);border-radius:999px;background:rgba(2,6,23,.65);color:#bfdbfe;font-size:13px;}
.grid{display:grid;grid-template-columns:repeat(4,1fr);gap:14px;margin-top:14px;}
.grid2{display:grid;grid-template-columns:1.05fr 1.05fr 1fr;gap:14px;margin-top:14px;align-items:stretch;}
.grid3{display:grid;grid-template-columns:1fr 1fr 1fr;gap:14px;margin-top:14px;align-items:stretch;}
.card{
    position:relative;
    background:linear-gradient(180deg,var(--panel2),var(--panel));
    border:1px solid var(--border);
    border-radius:18px;
    padding:16px;
    box-shadow:0 18px 44px rgba(0,0,0,.28), inset 0 1px 0 rgba(255,255,255,.04);
    backdrop-filter:blur(16px);
    overflow:hidden;
}
.card:before{content:"";position:absolute;left:0;right:0;top:0;height:1px;background:linear-gradient(90deg,transparent,rgba(125,211,252,.5),transparent);}
.card h2,.card h3{margin:0 0 12px;font-size:16px;letter-spacing:.1px;}
.card h3{font-size:13px;color:#e2e8f0;}
.big{font-size:30px;font-weight:900;color:var(--green);letter-spacing:.4px;}
.muted{color:var(--muted);font-size:12px;}
.metricRow{display:flex;align-items:flex-end;justify-content:space-between;gap:10px;}
.smallStat{font-size:12px;color:var(--muted);margin-top:6px;}
.statusGlow{position:absolute;right:14px;top:14px;width:9px;height:9px;border-radius:50%;background:var(--green);box-shadow:0 0 18px var(--green);}
.visualGrid{display:grid;grid-template-columns:260px 1fr 1fr;gap:14px;margin-top:14px;}
.donutWrap{display:flex;flex-direction:column;align-items:center;justify-content:center;min-height:240px;}
.donut{
    --p:0;
    width:168px;height:168px;border-radius:50%;
    background:conic-gradient(var(--green) calc(var(--p)*1%),rgba(148,163,184,.18) 0);
    display:grid;place-items:center;
    box-shadow:0 0 40px rgba(34,197,94,.18);
}
.donutInner{width:124px;height:124px;border-radius:50%;background:#07111f;display:grid;place-items:center;text-align:center;border:1px solid rgba(148,163,184,.18);}
.donutInner b{font-size:30px;color:#fff;}
.donutInner span{font-size:12px;color:var(--muted);}
.progressOuter{height:14px;border-radius:999px;background:#020617;border:1px solid rgba(148,163,184,.18);overflow:hidden;margin-top:10px;}
.progressInner{height:100%;width:0%;background:linear-gradient(90deg,var(--green),var(--cyan));border-radius:999px;transition:width .25s ease;box-shadow:0 0 18px rgba(56,189,248,.45);}
.graphCard canvas{width:100%;height:150px;display:block;margin-top:10px;background:rgba(2,6,23,.35);border-radius:14px;border:1px solid rgba(148,163,184,.14);}
.clientRow{display:flex;align-items:center;justify-content:space-between;background:rgba(2,6,23,.74);border-radius:12px;padding:10px 12px;margin:8px 0;border:1px solid rgba(148,163,184,.16);}
.badge{min-width:112px;text-align:center;padding:7px 12px;border-radius:999px;font-weight:900;font-size:12px;letter-spacing:.4px;}
.runBadge{background:#16a34a;color:white;box-shadow:0 0 18px rgba(34,197,94,.55);}
.pauseBadge{background:#dc2626;color:white;box-shadow:0 0 20px rgba(220,38,38,.70);animation:blink .75s infinite;}
.idleBadge{background:#64748b;color:white;}
.priorityPulse{animation:pulseBorder 1s infinite;}
@keyframes blink{50%{opacity:.50;}}
@keyframes pulseBorder{0%{box-shadow:0 0 0 rgba(239,68,68,0)}50%{box-shadow:0 0 26px rgba(239,68,68,.38)}100%{box-shadow:0 0 0 rgba(239,68,68,0)}}
.infoLine{display:flex;align-items:center;justify-content:space-between;padding:7px 0;border-bottom:1px solid rgba(148,163,184,.08);font-size:13px;}
.infoLine span:first-child{color:#cbd5e1;}.infoLine span:last-child{font-weight:700;color:#e0f2fe;}
.box{background:rgba(2,6,23,.70);padding:10px 12px;border-radius:10px;margin-top:8px;font-size:13px;line-height:1.35;border:1px solid rgba(148,163,184,.10);}
.osBox{display:grid;grid-template-columns:1fr;gap:7px;margin-top:8px;}
.osItem{background:rgba(2,6,23,.70);padding:8px 10px;border-radius:9px;font-size:12px;line-height:1.25;border-left:3px solid var(--cyan);white-space:nowrap;overflow:hidden;text-overflow:ellipsis;}
.osItem b{color:#e0f2fe;}
.thread{display:flex;align-items:center;justify-content:space-between;background:rgba(2,6,23,.72);margin:7px 0;padding:9px 10px;border-radius:10px;color:#dbeafe;font-size:12px;border:1px solid rgba(148,163,184,.10);}
.threadDot{width:9px;height:9px;border-radius:50%;display:inline-block;margin-right:7px;background:#64748b;}
.threadActive .threadDot{background:var(--green);box-shadow:0 0 14px rgba(34,197,94,.8);animation:blink .9s infinite;}
.threadIdle{opacity:.72;}
.log{background:rgba(2,6,23,.72);margin:7px 0;padding:9px 10px;border-radius:9px;color:#dbeafe;font-size:12px;border-left:3px solid rgba(96,165,250,.75);}
.footer{margin-top:18px;color:#94a3b8;font-size:12px;}
@media(max-width:1100px){.grid{grid-template-columns:repeat(2,1fr)}.grid2,.grid3,.visualGrid{grid-template-columns:1fr}}
</style>
</head>
<body>
<div class="container">
    <div class="topbar">
        <div class="titleBlock">
            <h1>Live Multithreaded Web Server Dashboard</h1>
        </div>
    </div>

    <div class="grid">
        <div class="card"><span class="statusGlow"></span><h3>Mode</h3><div id="mode" class="big">MULTITHREADED</div></div>
        <div class="card"><h3>Server Status</h3><div id="serverStatus" class="big">NORMAL</div></div>
        <div class="card"><h3>Total Requests</h3><div id="total" class="big">0</div><div class="smallStat">Limit: <span id="requestLimit">0</span></div></div>
        <div class="card"><h3>Rejected / Overload</h3><div id="rejected" class="big">0</div><div class="smallStat">Protected mode counter</div></div>
    </div>

    <div class="grid">
        <div class="card"><h3>Normal Requests</h3><div id="normalReq" class="big">0</div></div>
        <div class="card"><h3>Priority Requests</h3><div id="priorityReq" class="big">0</div></div>
        <div class="card"><h3>Queue Load</h3><div><span id="queueSize" class="big">0</span> / <span id="maxQueue">0</span></div><div class="progressOuter"><div id="queueBar" class="progressInner"></div></div></div>
        <div class="card"><h3>Threads</h3><div><span id="activeThreads" class="big">0</span> / <span id="maxThreads">0</span></div><div class="smallStat">Active worker threads</div></div>
    </div>

    <div class="visualGrid">
        <div class="card donutWrap">
            <h2>Circular Thread Usage</h2>
            <div id="threadDonut" class="donut"><div class="donutInner"><div><b id="threadPercent">0%</b><br><span>CPU thread usage</span></div></div></div>
        </div>
        <div class="card graphCard">
            <h2>Live Request Graph</h2>
            <canvas id="requestGraph" width="600" height="150"></canvas>
            <div class="smallStat">Shows total request growth in real time.</div>
        </div>
        <div id="priorityCard" class="card">
            <h2>Priority Animation</h2>
            <p>Priority: <span id="priority">WAITING</span></p>
            <p>Normal Clients: <span id="normal">ACTIVE</span></p>
            <div class="box">When priority runs, normal clients pause and turn red.</div>
        </div>
    </div>

    <div class="grid2">
        <div class="card">
            <h2>Latest Client</h2>
            <div class="infoLine"><span>Client</span><span id="client">None</span></div>
            <div class="infoLine"><span>Type</span><span id="type">None</span></div>
            <div class="infoLine"><span>Thread</span><span id="thread">None</span></div>
            <div class="infoLine"><span>Response</span><span id="response">0 ms</span></div>
            <div class="infoLine"><span>Devices</span><span id="devices">0</span></div>
        </div>

        <div class="card">
            <h2>Client Status Color Demo</h2>
            <div class="clientRow"><span>Client 1</span><span id="client1Badge" class="badge runBadge">RUNNING</span></div>
            <div class="clientRow"><span>Client 2</span><span id="client2Badge" class="badge runBadge">RUNNING</span></div>
            <div class="clientRow"><span>Client 3</span><span id="client3Badge" class="badge runBadge">RUNNING</span></div>
            <div class="clientRow"><span>Priority</span><span id="priorityBadge" class="badge idleBadge">IDLE</span></div>
        </div>

        <div class="card">
            <h2>OS Concepts</h2>
            <div class="osBox">
                <div class="osItem"><b>Thread Pool:</b> Reuses worker threads.</div>
                <div class="osItem"><b>Mutex:</b> Prevents race conditions.</div>
                <div class="osItem"><b>Condition Variable:</b> Pause / Resume clients.</div>
                <div class="osItem"><b>Producer-Consumer:</b> Queue handling.</div>
                <div class="osItem"><b>Priority Scheduling:</b> Priority first.</div>
                <div class="osItem"><b>Hardware Concurrency:</b> Uses max CPU threads.</div>
                <div class="osItem"><b>Overload Protection:</b> Prevents crash.</div>
                <div class="osItem"><b>MPI:</b> Future distributed processing.</div>
            </div>
        </div>
    </div>

    <div class="grid3">
        <div class="card"><h2>Thread Activity Animation</h2><div id="threadHtml"></div></div>
        <div class="card"><h2>Queue / Limit Meaning</h2><div class="box">0 - 40%: Normal</div><div class="box">40 - 80%: High load</div><div class="box">80%+: Overload</div><div class="box">After limit: requests rejected, server protected.</div></div>
        <div class="card"><h2>Live Event Log</h2><div id="events"></div></div>
    </div>

    <div class="footer">Demo URLs: /data /client-status /file /pause /priority | Status API: /status</div>
</div>

<script>
let requestHistory=[];

function setText(id,value){
    let e=document.getElementById(id);
    if(e)e.innerText=value;
}

function setHtml(id,value){
    let e=document.getElementById(id);
    if(e)e.innerHTML=value;
}

function setBadge(el,text,cls){
    if(el){el.innerText=text;el.className='badge '+cls;}
}

function drawGraph(){
    let canvas=document.getElementById('requestGraph');
    if(!canvas)return;
    let ctx=canvas.getContext('2d');
    let w=canvas.width,h=canvas.height;
    ctx.clearRect(0,0,w,h);
    ctx.strokeStyle='rgba(148,163,184,.18)';
    ctx.lineWidth=1;
    for(let i=1;i<5;i++){let y=i*h/5;ctx.beginPath();ctx.moveTo(0,y);ctx.lineTo(w,y);ctx.stroke();}
    if(requestHistory.length<2)return;
    let max=Math.max(...requestHistory,1);
    ctx.beginPath();
    ctx.lineWidth=3;
    ctx.strokeStyle='#38bdf8';
    requestHistory.forEach((v,i)=>{
        let x=(i/(requestHistory.length-1))*w;
        let y=h-(v/max)*(h-14)-7;
        if(i===0)ctx.moveTo(x,y);else ctx.lineTo(x,y);
    });
    ctx.stroke();
    let grad=ctx.createLinearGradient(0,0,0,h);
    grad.addColorStop(0,'rgba(56,189,248,.28)');
    grad.addColorStop(1,'rgba(56,189,248,0)');
    ctx.lineTo(w,h);ctx.lineTo(0,h);ctx.closePath();ctx.fillStyle=grad;ctx.fill();
}

async function updateDashboard(){
    try{
        let r=await fetch('/status?time='+Date.now());
        let d=await r.json();

        for(let k of ['mode','serverStatus','total','normalReq','priorityReq','rejected','queueSize','maxQueue','maxThreads','activeThreads','requestLimit','devices','client','type','thread','response','priority','normal']){
            setText(k,d[k]);
        }
        setHtml('threadHtml',d.threadHtml||'');
        setHtml('events',d.events||'');

        let active=parseInt(d.activeThreads||'0');
        let max=parseInt(d.maxThreads||'1');
        let threadPercent=Math.min(100,Math.round((active/Math.max(max,1))*100));
        let donut=document.getElementById('threadDonut');
        if(donut)donut.style.setProperty('--p',threadPercent);
        setText('threadPercent',threadPercent+'%');

        let q=parseInt(d.queueSize||'0');
        let mq=parseInt(d.maxQueue||'1');
        let qPct=Math.min(100,Math.round((q/Math.max(mq,1))*100));
        let qbar=document.getElementById('queueBar');
        if(qbar)qbar.style.width=qPct+'%';

        let total=parseInt(d.total||'0');
        requestHistory.push(total);
        if(requestHistory.length>36)requestHistory.shift();
        drawGraph();

        let priorityActive=(d.priority==='RUNNING'||d.normal==='PAUSED');
        let c1=document.getElementById('client1Badge');
        let c2=document.getElementById('client2Badge');
        let c3=document.getElementById('client3Badge');
        let pb=document.getElementById('priorityBadge');
        let priorityCard=document.getElementById('priorityCard');

        if(priorityActive){
            setBadge(c1,'PAUSED','pauseBadge');
            setBadge(c2,'PAUSED','pauseBadge');
            setBadge(c3,'PAUSED','pauseBadge');
            setBadge(pb,'RUNNING','runBadge');
            if(priorityCard)priorityCard.classList.add('priorityPulse');
            let p=document.getElementById('priority');
            let n=document.getElementById('normal');
            if(p){p.innerText='RUNNING';p.style.color='#22c55e';}
            if(n){n.innerText='PAUSED';n.style.color='#ef4444';}
        }else{
            setBadge(c1,'RUNNING','runBadge');
            setBadge(c2,'RUNNING','runBadge');
            setBadge(c3,'RUNNING','runBadge');
            setBadge(pb,'IDLE','idleBadge');
            if(priorityCard)priorityCard.classList.remove('priorityPulse');
            let p=document.getElementById('priority');
            let n=document.getElementById('normal');
            if(p){p.innerText='WAITING';p.style.color='#facc15';}
            if(n){n.innerText='ACTIVE';n.style.color='#22c55e';}
        }

        let server=document.getElementById('serverStatus');
        if(server){
            if((d.serverStatus||'').includes('OVERLOAD')||(d.serverStatus||'').includes('LIMIT'))server.style.color='#ef4444';
            else if((d.serverStatus||'').includes('HIGH'))server.style.color='#facc15';
            else server.style.color='#22c55e';
        }
    }catch(e){console.log(e);}
}

setInterval(updateDashboard,150);
updateDashboard();
</script>
</body>
</html>)HTML";

    size_t pos = html.find("\n\n");

    if (pos != std::string::npos) {
        html.replace(pos, 2, "\r\n\r\n");
    }

    return html;
}

void sendText(SocketType clientSocket, const std::string& body) {
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n\r\n" + body;

    send(clientSocket, response.c_str(), (int)response.size(), 0);
}

void ClientHandler::handle(SocketType clientSocket, bool isPriority, PriorityControl& priorityControl, int workerId) {
    auto start = std::chrono::high_resolution_clock::now();

    char buffer[4096] = {0};
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead <= 0) {
        closeSocket(clientSocket);
        return;
    }

    std::string request(buffer);
    std::string path = getPath(request);

    if (path.find("/status") == 0 && path.find("name=") == std::string::npos) {
        std::string json = getStatusJson();

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Cache-Control: no-cache\r\n"
            "Connection: close\r\n\r\n" + json;

        send(clientSocket, response.c_str(), (int)response.size(), 0);
        closeSocket(clientSocket);
        return;
    }

    if (path == "/" || path.find("/index") == 0) {
        std::string response = dashboardHtml();
        send(clientSocket, response.c_str(), (int)response.size(), 0);
        closeSocket(clientSocket);
        return;
    }

    std::string clientName = getQueryValue(path, "name");
    std::string clientType = getQueryValue(path, "type");

    if (clientName.empty()) {
        clientName = getHeaderValue(request, "Client-Name");
    }

    if (clientType.empty()) {
        clientType = getHeaderValue(request, "Client-Type");
    }

    if (path.find("/normal") == 0) {
        clientType = "Normal";

        if (clientName == "Browser") {
            clientName = "PhoneNormal";
        }
    }

    if (path.find("/priority") == 0 || path.find("/pause") == 0) {
        isPriority = true;
        clientType = "Priority";

        if (clientName == "Browser") {
            clientName = "PhonePriority";
        }
    }

    if (workerId >= (int)threadStatus.size()) {
        std::lock_guard<std::mutex> lock(dashboardMutex);
        threadStatus.resize(workerId + 2, "IDLE");
    }

    std::string threadName = "Thread-" + std::to_string(workerId);

    {
        std::lock_guard<std::mutex> lock(dashboardMutex);

        totalRequests++;
        connectedDevices++;

        lastClient = clientName;
        lastType = clientType;
        lastThread = threadName;

        threadStatus[workerId] = "ACTIVE";

        if (totalRequests > requestLimit) {
            serverStatus = "LIMIT REACHED";
        } else if (currentQueueSize > (maxQueueSize * 8 / 10)) {
            serverStatus = "OVERLOAD";
        } else if (currentQueueSize > (maxQueueSize * 4 / 10)) {
            serverStatus = "HIGH LOAD";
        } else {
            serverStatus = "NORMAL";
        }
    }

    addEvent(clientName + " assigned to " + threadName);

    if (isPriority || path.find("/pause") == 0 || path.find("/priority") == 0) {
        {
            std::lock_guard<std::mutex> lock(dashboardMutex);

            priorityRequests++;
            priorityVisualActive = true;
            priorityStatus = "RUNNING";
            normalStatus = "PAUSED";
            threadStatus[workerId] = "PROCESSING PRIORITY";
        }

        addEvent("Priority request received");
        addEvent("Normal clients paused - RED");

        priorityControl.pauseNormalClients();

        std::this_thread::sleep_for(std::chrono::seconds(3));

        priorityControl.resumeNormalClients();

        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        {
            std::lock_guard<std::mutex> lock(dashboardMutex);

            responseTime = std::to_string(ms) + " ms";
            priorityVisualActive = false;
            priorityStatus = "WAITING";
            normalStatus = "ACTIVE";
            threadStatus[workerId] = "COMPLETED";
        }

        addEvent("Priority completed - normal clients resumed");

        sendText(clientSocket, "<h1>Priority Request Completed</h1>");

        Logger::log("Priority request processed for: " + clientName);
    } else {
        priorityControl.waitIfPaused();

        {
            std::lock_guard<std::mutex> lock(dashboardMutex);

            normalRequests++;

            if (!priorityVisualActive) {
                priorityStatus = "WAITING";
                normalStatus = "ACTIVE";
            }
        }

        addEvent("Normal request processing: " + clientName);

        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        {
            std::lock_guard<std::mutex> lock(dashboardMutex);

            responseTime = std::to_string(ms) + " ms";
            threadStatus[workerId] = "COMPLETED";
        }

        addEvent("Request completed for " + clientName);

        sendText(clientSocket, "<h1>Normal Request Completed</h1>");

        Logger::log("Normal request processed for: " + clientName);
    }

    {
        std::lock_guard<std::mutex> lock(dashboardMutex);

        if (workerId < (int)threadStatus.size()) {
            threadStatus[workerId] = "IDLE";
        }
    }

    closeSocket(clientSocket);
}
