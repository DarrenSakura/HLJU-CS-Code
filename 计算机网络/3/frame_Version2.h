#pragma once

// 模拟以太网数据帧结构
struct EthernetFrame {
    char srcMAC;       // 源 MAC 地址 (用单个字符模拟, 如 'A')
    char destMAC;      // 目的 MAC 地址 (用单个字符模拟, 如 'B', 'F'表示广播)
    int inPort;        // 模拟该主机连接在网桥的哪个端口 (如 1, 2, 3)
    char payload[64];  // 数据载荷
};