#include <iostream>
#include <winsock2.h>
#include <string>
#include <time.h>
#include "frame.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(8888);
    recvAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 本机测试

    int addrLen = sizeof(recvAddr);
    int V_S = 0; // (2) V(S) <- 0

    cout << "===== 发方 (Sender) 启动 =====" << endl;

    while (true) {
        // (1) 从主机取一个数据帧，送交发送缓存
        string input;
        cout << "\n请输入要发送的数据 (输入 quit 退出): ";
        getline(cin, input);
        if (input == "quit") break;

        Frame sendFrame;
        memset(&sendFrame, 0, sizeof(sendFrame));
        sendFrame.type = 0; 
        strcpy(sendFrame.data, input.c_str());

        // (3) N(S) <- V(S)
        sendFrame.seq = V_S;
        sendFrame.crc = CalculateCRC(sendFrame); // 计算校验和

        bool ackReceived = false;

        while (!ackReceived) {
            // (4) 将发送缓存中的数据帧发送出去
            cout << "[发送] 数据帧 N(S)=" << sendFrame.seq << ", 数据: " << sendFrame.data << endl;
            sendto(sendSocket, (char*)&sendFrame, sizeof(sendFrame), 0, (sockaddr*)&recvAddr, addrLen);

            // (5) 设置超时计时器 & (6) 等待
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(sendSocket, &readfds);

            timeval timeout;
            timeout.tv_sec = 2; // 超时时间 2 秒
            timeout.tv_usec = 0;

            // 使用 select 进行超时检测
            int ret = select(0, &readfds, NULL, NULL, &timeout);

            if (ret > 0) {
                // 有数据到来，接收确认帧
                Frame ackFrame;
                recvfrom(sendSocket, (char*)&ackFrame, sizeof(ackFrame), 0, (sockaddr*)&recvAddr, &addrLen);

                // (7) 收到确认帧 ACKn
                if (ackFrame.type == 1) {
                    cout << "[接收] 收到确认帧 ACK, n=" << ackFrame.seq << endl;
                    
                    // 若 n = 1 - V(S)
                    if (ackFrame.seq == 1 - V_S) {
                        cout << " -> 确认无误，准备发送下一个数据。" << endl;
                        // V(S) <- [1 - V(S)]
                        V_S = 1 - V_S;
                        ackReceived = true; // 跳出循环，转到 (3) 开始下一���发送
                    } else {
                        // 否则，丢弃这个确认帧，转到(6)（此处循环会继续，由于没跳出）
                        cout << " -> 序号不匹配，丢弃此ACK，继续等待..." << endl;
                    }
                }
            } else if (ret == 0) {
                // (8) 若超时计时器时间到，则转到(4) (继续 while 循环即重新发送)
                cout << "[超时] 未收到ACK，准备重传..." << endl;
            }
        }
    }

    closesocket(sendSocket);
    WSACleanup();
    return 0;
}