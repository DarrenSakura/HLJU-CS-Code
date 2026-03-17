#include <iostream>
#include <winsock2.h>
#include <string>
#include "frame.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET hostSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    sockaddr_in bridgeAddr;
    bridgeAddr.sin_family = AF_INET;
    bridgeAddr.sin_port = htons(8888);
    bridgeAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 网桥的IP地址

    cout << "===== 主机模拟器 (Host) =====" << endl;

    while (true) {
        EthernetFrame frame;
        
        cout << "\n请输入本机的源MAC (如 'A'): ";
        cin >> frame.srcMAC;
        cout << "请输入本机连接网桥的端口号 (如 1, 2, 3): ";
        cin >> frame.inPort;
        cout << "请输入要发送的目的MAC (如 'B', 广播填 'F'): ";
        cin >> frame.destMAC;
        cout << "请输入数据载荷: ";
        cin >> frame.payload;

        sendto(hostSocket, (char*)&frame, sizeof(frame), 0, (sockaddr*)&bridgeAddr, sizeof(bridgeAddr));
        cout << "数据帧已发送给网桥！" << endl;
    }

    closesocket(hostSocket);
    WSACleanup();
    return 0;
}