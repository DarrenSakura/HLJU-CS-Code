#include <iostream>
#include <winsock2.h>
#include <map>
#include <vector>
#include <thread>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// RIP 路由表项结构
struct RouteEntry {
    char dest;      // 目的路由器ID
    char nextHop;   // 下一跳路由器ID
    int distance;   // 距离(跳数)，最大16表示不可达
    int timer;      // 计时器(秒)，用于老化机制
};

// RIP 传输的报文数据结构
struct RIPPacket {
    char senderId;  // 发送此报文的路由器ID
    int entryCount; // 路由表项数量
    struct {
        char dest;
        int distance;
    } entries[20];
};

// 全局变量
char myId;
int myPort;
map<char, RouteEntry> routingTable; // 核心：路由表
vector<pair<char, int>> neighbors;  // 邻居列表 <邻居ID, 邻居端口>
mutex tableMutex;                   // 互斥锁保护路由表

const int INFINITY_DIST = 16; // 16跳表示不可达
const int UPDATE_INTERVAL = 5; // 每5秒广播一次路由表
const int TIMEOUT_INTERVAL = 15; // 15秒未更新则认为不可达

// 打印当前路由表
void PrintTable() {
    cout << "\n=== 路由器 " << myId << " 的路由表 ===" << endl;
    cout << "目的网段\t下一跳\t跳数(距离)\t状态" << endl;
    for (auto const& pair : routingTable) {
        const RouteEntry& entry = pair.second;
        string status = (entry.distance >= INFINITY_DIST) ? "不可达(16)" : to_string(entry.distance);
        cout << "   " << entry.dest << "\t\t   " << entry.nextHop << "\t  " << status << "\t\t" << entry.timer << "s" << endl;
    }
    cout << "=========================" << endl;
}

// Timer 定时器线程函数：负责周期性发送更新和老化机制
void TimerThreadFunc(SOCKET sock) {
    int tick = 0;
    while (true) {
        Sleep(1000); // 1秒滴答一次
        tick++;

        lock_guard<mutex> lock(tableMutex);
        bool changed = false;

        // 1. 老化机制：检查是否有超时的路由
        for (auto& pair : routingTable) {
            if (pair.first == myId) continue; // 自己到自己不老化
            
            pair.second.timer++;
            if (pair.second.timer >= TIMEOUT_INTERVAL && pair.second.distance < INFINITY_DIST) {
                pair.second.distance = INFINITY_DIST; // 标记为不可达
                changed = true;
                cout << "\n[老化机制] 到达 " << pair.first << " 的路由已超时，标记为不可达！" << endl;
            }
        }

        if (changed) PrintTable();

        // 2. 周期性发送更新：每 5 秒向所有邻居发送一次路由表
        if (tick % UPDATE_INTERVAL == 0) {
            RIPPacket pkt;
            pkt.senderId = myId;
            pkt.entryCount = 0;

            for (auto const& pair : routingTable) {
                pkt.entries[pkt.entryCount].dest = pair.first;
                pkt.entries[pkt.entryCount].distance = pair.second.distance;
                pkt.entryCount++;
            }

            // 发送给所有配置的直连邻居
            for (auto const& neighbor : neighbors) {
                sockaddr_in destAddr;
                destAddr.sin_family = AF_INET;
                destAddr.sin_port = htons(neighbor.second);
                destAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

                sendto(sock, (char*)&pkt, sizeof(pkt), 0, (sockaddr*)&destAddr, sizeof(destAddr));
            }
            cout << "\n[Timer] 发送周期性 RIP 更新报文..." << endl;
        }
    }
}

int main() {
    // 1. 初始化设置
    cout << "请输入本路由器的 ID (如 A, B, C): ";
    cin >> myId;
    cout << "请输入本路由器的绑定端口 (如 8001, 8002): ";
    cin >> myPort;

    // 初始路由表包含自己，距离为0
    routingTable[myId] = {myId, myId, 0, 0};

    int neighborCount;
    cout << "请输入直接相邻的邻居数量: ";
    cin >> neighborCount;
    for (int i = 0; i < neighborCount; ++i) {
        char nid; int nport;
        cout << "请输入第 " << i + 1 << " 个邻居的 ID 和 端口 (例如: B 8002): ";
        cin >> nid >> nport;
        neighbors.push_back({nid, nport});
        // 将直连邻居加入路由表，距离为1
        routingTable[nid] = {nid, nid, 1, 0}; 
    }

    // 2. 初始化 Socket
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(myPort);
    localAddr.sin_addr.s_addr = INADDR_ANY;

    bind(recvSocket, (sockaddr*)&localAddr, sizeof(localAddr));

    cout << "\n路由器 " << myId << " 已启动，正在监听端口 " << myPort << "...\n" << endl;
    PrintTable();

    // 3. 启动 Timer 线程
    thread timerThread(TimerThreadFunc, recvSocket);
    timerThread.detach();

    // 4. 主线程：接收报文并执行 RIP 更新算法 (Bellman-Ford)
    sockaddr_in senderAddr;
    int addrLen = sizeof(senderAddr);
    
    while (true) {
        RIPPacket pkt;
        int bytes = recvfrom(recvSocket, (char*)&pkt, sizeof(pkt), 0, (sockaddr*)&senderAddr, &addrLen);
        
        if (bytes > 0) {
            lock_guard<mutex> lock(tableMutex);
            bool tableChanged = false;

            cout << "\n[接收] 收到来自路由器 " << pkt.senderId << " 的 RIP 更新。" << endl;

            // 遍历收到的路由表项
            for (int i = 0; i < pkt.entryCount; ++i) {
                char dest = pkt.entries[i].dest;
                int recvDist = pkt.entries[i].distance;
                int newDist = (recvDist >= INFINITY_DIST) ? INFINITY_DIST : recvDist + 1; // 经过邻居，跳数+1

                // 如果目的地址不在我的路由表中，且不是不可达，则添加
                if (routingTable.find(dest) == routingTable.end()) {
                    if (newDist < INFINITY_DIST) {
                        routingTable[dest] = {dest, pkt.senderId, newDist, 0};
                        tableChanged = true;
                        cout << " -> 学习到新路由: 目的 " << dest << ", 下一跳 " << pkt.senderId << ", 距离 " << newDist << endl;
                    }
                } else {
                    RouteEntry& myEntry = routingTable[dest];
                    
                    // 如果这条路由本来就是从该邻居学来的 (下一跳相同)
                    if (myEntry.nextHop == pkt.senderId) {
                        // 无论距离变大变小都必须更新，并重置定时器
                        if (myEntry.distance != newDist) {
                            myEntry.distance = newDist;
                            tableChanged = true;
                            cout << " -> 更新拓扑变化: 目的 " << dest << ", 距离变为 " << newDist << endl;
                        }
                        myEntry.timer = 0; // 重置老化计时器
                    } 
                    // 如果是从其他邻居收到的，且发现了一条更短的路径
                    else if (newDist < myEntry.distance) {
                        myEntry.nextHop = pkt.senderId;
                        myEntry.distance = newDist;
                        myEntry.timer = 0;
                        tableChanged = true;
                        cout << " -> 发现更优路径: 目的 " << dest << ", 新下一跳 " << pkt.senderId << ", 距离 " << newDist << endl;
                    }
                }
            }

            if (tableChanged) {
                PrintTable();
            }
        }
    }

    closesocket(recvSocket);
    WSACleanup();
    return 0;
}