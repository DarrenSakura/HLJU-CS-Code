#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    // 1. 初始化 Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup 失败!" << endl;
        return 1;
    }

    // 2. 创建套接字
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "创建套接字失败!" << endl;
        WSACleanup();
        return 1;
    }

    // 3. 指定要连接的服务器地址和端口
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888); // 必须与服务器端口一致
    
    // 如果是本机通信，使用 "127.0.0.1"。
    // 如果是两台计算机通信，请将这里替换为服务器所在计算机的局域网 IP 地址 (例如 "192.168.1.100")
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    // 4. 连接到服务器
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "连接服务器失败! 请确认服务器是否已启动。" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "成功连接到服务器!" << endl;

    // 5. 数据传输
    char buffer[1024];
    string message;
    while (true) {
        cout << "请输入要发送的信息 (输入 'quit' 退出): ";
        getline(cin, message);

        if (message == "quit") {
            break;
        }

        // 发送数据
        send(clientSocket, message.c_str(), message.length(), 0);

        // 接收服务器响应
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            cout << "收到回复: " << buffer << endl;
        }
    }

    // 6. 关闭套接字
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}