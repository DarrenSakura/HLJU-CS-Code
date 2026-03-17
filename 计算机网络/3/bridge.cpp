#include <iostream>
#include <winsock2.h>
#include <map>
#include <thread>
#include <mutex>
#include <vector>
#include "frame.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

// 网桥转发表项结构
struct TableEntry {
    int port;
    int ttl; // 存活时间 (Time To Live)，用于 Timer 老化
};

map<char, TableEntry> macTable; // 网桥站表 (MAC -> 表项)
mutex tableMutex;               // 互斥锁，保护站表在多线程下的安全

const int MAX_TTL = 15; // 初始存活时间设置为 15 秒

// 打印当前网桥站表
void PrintTable() {
    cout << "\n--- 当前网桥站表 (MAC Table) ---" << endl;
    cout << "MAC地址\t端口\tTTL(剩余时间)" << endl;
    if (macTable.empty()) {
        cout << "(空)" << endl;
    } else {
        for (auto const& pair : macTable) {
            cout << "   " << pair.first << "\t  " << pair.second.port << "\t  " << pair.second.ttl << "s" << endl;
        }
    }
    cout << "--------------------------------\n" << endl;
}

// Timer 定时器线程函数：负责老化机制
void TimerThreadFunc() {
    while (true) {
        Sleep(1000); // 每 1 秒执行一次

        lock_guard<mutex> lock(tableMutex); // 加锁
        bool tableChanged = false;
        
        // 遍历并减少 TTL，移除超时的条目
        for (auto it = macTable.begin(); it != macTable.end(); ) {
            it->second.ttl--;
            if (it->second.ttl <= 0) {
                cout << "\n[老化机制] MAC地址 " << it->first << " 超时，已从站表中删除！" << endl;
                it = macTable.erase(it); // 删除过期条目
                tableChanged = true;
            } else {
                ++it;
            }
        }

        if (tableChanged) {
            PrintTable();
        }
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET bridgeSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in bridgeAddr;
    bridgeAddr.sin_family = AF_INET;
    bridgeAddr.sin_port = htons(8888);
    bridgeAddr.sin_addr.s_addr = INADDR_ANY;

    bind(bridgeSocket, (sockaddr*)&bridgeAddr, sizeof(bridgeAddr));

    cout << "===== 网桥 (Bridge) 已启动，端口: 8888 =====" << endl;
    PrintTable();

    // 启动 Timer 定时器线程
    thread timerThread(TimerThreadFunc);
    timerThread.detach(); // 后台运行

    sockaddr_in hostAddr;
    int addrLen = sizeof(hostAddr);

    // 主循环：接收数据并处理
    while (true) {
        EthernetFrame frame;
        int bytes = recvfrom(bridgeSocket, (char*)&frame, sizeof(frame), 0, (sockaddr*)&hostAddr, &addrLen);
        
        if (bytes > 0) {
            lock_guard<mutex> lock(tableMutex); // 加锁处理站表
            
            cout << "\n[接收帧] 从端口 " << frame.inPort << " 收到帧: 源MAC=" << frame.srcMAC 
                 << ", 目的MAC=" << frame.destMAC << ", 数据: " << frame.payload << endl;

            // 1. 自学习机制 (更新源 MAC)
            if (macTable.find(frame.srcMAC) == macTable.end()) {
                cout << "[学习机制] 发现新MAC地址 " << frame.srcMAC << "，记录入表！" << endl;
            } else {
                cout << "[学习机制] MAC地址 " << frame.srcMAC << " 已存在，刷新TTL和端口！" << endl;
            }
            macTable[frame.srcMAC] = {frame.inPort, MAX_TTL};
            PrintTable();

            // 2. 转发机制决策 (查找目的 MAC)
            if (frame.destMAC == 'F') { // 假设 'F' 为广播地址
                cout << "[转发决策] 目的地址为广播地址，向除 " << frame.inPort << " 以外的所有端口洪泛 (Flooding)！" << endl;
            } else {
                auto it = macTable.find(frame.destMAC);
                if (it != macTable.end()) {
                    // 在表中找到了目的 MAC
                    if (it->second.port == frame.inPort) {
                        cout << "[转发决策] 目的MAC " << frame.destMAC << " 也在端口 " << frame.inPort 
                             << "，丢弃此帧 (Filtering)！" << endl;
                    } else {
                        cout << "[转发决策] 目的MAC " << frame.destMAC << " 在端口 " << it->second.port 
                             << "，将帧转发至端口 " << it->second.port << " (Forwarding)！" << endl;
                    }
                } else {
                    // 表中没有目的 MAC
                    cout << "[转发决策] 未知目的MAC " << frame.destMAC << "，向除 " << frame.inPort 
                         << " 以外的所有端口洪泛 (Flooding)！" << endl;
                }
            }
        }
    }

    closesocket(bridgeSocket);
    WSACleanup();
    return 0;
}