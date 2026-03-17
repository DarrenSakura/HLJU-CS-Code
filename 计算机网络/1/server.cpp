#include <iostream>
#include <winsock2.h>
#include <string>

// 告诉编译器链接 Winsock 库
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    // 1. 初始化 Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup 失败!" << endl;
        return 1;
    }

    // 2. 创建套接字 (AF_INET: IPv4, SOCK_STREAM: TCP协议)
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "创建套接字失败!" << endl;
        WSACleanup();
        return 1;
    }

    // 3. 绑定��接字到本地IP和端口
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888); // 端口号 8888
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 监听任意本地IP地址

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "绑定失败!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 4. 开始监听 (最大等待连接数为 5)
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        cout << "监听失败!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "服务器已启动，等待客户端连接 (端口: 8888)..." << endl;

    // 5. 接受客户端连接
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        cout << "接受连接失败!" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "客户端已连接!" << endl;

    // 6. 数据传输 (接收和发送)
    char buffer[1024] = {0};
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        // 接收数据
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            cout << "客户端说: " << buffer << endl;
            
            // 简单的回显/回复
            string response = "服务器已收到你的消息: " + string(buffer);
            send(clientSocket, response.c_str(), response.length(), 0);
        } else if (bytesReceived == 0) {
            cout << "客户端断开连接。" << endl;
            break;
        } else {
            cout << "接收失败!" << endl;
            break;
        }
    }

    // 7. 关闭套接字
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}