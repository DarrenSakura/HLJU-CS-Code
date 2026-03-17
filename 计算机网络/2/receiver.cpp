#include <iostream>
#include <winsock2.h>
#include <stdlib.h>
#include <time.h>
#include "frame.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    sockaddr_in localAddr;
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(8888);
    localAddr.sin_addr.s_addr = INADDR_ANY;

    bind(recvSocket, (sockaddr*)&localAddr, sizeof(localAddr));

    cout << "===== 收方 (Receiver) 启动，等待数据 =====" << endl;

    srand(time(NULL)); // 初始化随机数种子，用于模拟丢包
    int V_R = 0; // (1) V(R) <- 0

    sockaddr_in senderAddr;
    int addrLen = sizeof(senderAddr);

    // (2) 等待
    while (true) {
        Frame recvFrame;
        int bytes = recvfrom(recvSocket, (char*)&recvFrame, sizeof(recvFrame), 0, (sockaddr*)&senderAddr, &addrLen);
        
        if (bytes > 0 && recvFrame.type == 0) {
            // 模拟 20% 的网络丢包或CRC校验错误
            if (rand() % 100 < 20) {
                cout << "[模拟网络波动] 发生 CRC 错误或丢包，丢弃此数据帧。" << endl;
                continue; // 丢弃此数据帧，转到 (2)
            }

            // (3) 收到一个数据帧
            cout << "\n[接收] 收到数据帧 N(S)=" << recvFrame.seq << endl;
            
            // 若 CRC校验无误 (此处简单验证)
            if (recvFrame.crc == CalculateCRC(recvFrame)) {
                
                // 若 N(S) = V(R)
                if (recvFrame.seq == V_R) {
                    // (4) 将收到的数据帧中的数据部分送交上层软件
                    cout << " -> 校验正确且序号匹配，提取数据: " << recvFrame.data << endl;
                    
                    // (5) V(R) <- [1 - V(R)]
                    V_R = 1 - V_R;
                } else {
                    // 否则丢弃此数据帧，转到(6) (发送ACK)
                    cout << " -> 发现重复数据帧，丢弃数据，但重发ACK。" << endl;
                }

                // (6) n <- V(R)
                int n = V_R;
                Frame ackFrame;
                ackFrame.type = 1;
                ackFrame.seq = n;
                
                // 发送确认帧 ACKn
                cout << "[发送] 回送确认帧 ACK n=" << ackFrame.seq << endl;
                sendto(recvSocket, (char*)&ackFrame, sizeof(ackFrame), 0, (sockaddr*)&senderAddr, addrLen);
                
                // 转到(2) (循环回到起点)
            } else {
                // 否则丢弃此数据帧，然后转到(2)
                cout << " -> CRC 校验失败，丢弃此数据帧！" << endl;
            }
        }
    }

    closesocket(recvSocket);
    WSACleanup();
    return 0;
}