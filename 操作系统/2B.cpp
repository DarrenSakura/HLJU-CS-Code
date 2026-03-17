#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <map>
#include <queue>
#include <set>

using namespace std;

// 内存块大小和数量
#define BLOCK_SIZE 1024
#define MEM_SIZE 64

// 页表项结构
struct PageTableEntry {
    int frame_number;    // 物理块号
    bool valid;          // 有效位（是否在内存中）
    bool dirty;          // 修改位
    int access_time;     // 访问时间（用于LRU）
    int load_time;       // 加载时间（用于FIFO）

    PageTableEntry() : frame_number(-1), valid(false), dirty(false),
        access_time(0), load_time(0) {}
};

// 进程控制块（扩充版本）
struct PCB {
    string name;
    int size;                    // 进程大小（字节）
    int page_count;              // 页面数量
    vector<PageTableEntry> page_table;  // 页表
    set<int> allocated_frames;   // 分配给该进程的内存帧集合
    queue<int> fifo_queue;       // 进程专属的FIFO队列
    PCB* next;

    PCB(string n, int s) : name(n), size(s), next(nullptr) {
        page_count = ceil(static_cast<double>(s) / BLOCK_SIZE);
        page_table.resize(page_count);
    }
};

// 分页存储管理系统类
class PagingMemoryManager {
private:
    char bitmap[MEM_SIZE / 8];        // 内存位示图
    char swap_bitmap[MEM_SIZE * 2 / 8]; // 置换空间位示图（1.5-2倍内存）
    int swap_size;
    int time_counter;
    PCB* current_process;
    vector<PCB*> processes;

    // 计算对数（用于地址转换）
    int mylog2(int size) {
        return static_cast<int>(ceil(log10(size) / log10(2)));
    }

    // 获取位示图的某一位
    int getbit(char b, int bit_no) {
        char mask = static_cast<char>(1) << bit_no;
        return (b & mask) ? 1 : 0;
    }

    // 设置位示图的某一位
    void setbit(char* b, int bit_no, int flag) {
        char mask = static_cast<char>(1) << bit_no;
        if (flag) {
            *b = *b | mask;
        }
        else {
            mask = ~mask;
            *b = *b & mask;
        }
    }

    // 在位示图中查找空闲块
    int find_free_block(char* bitmap, int bitmap_size) {
        for (int i = 0; i < bitmap_size; i++) {
            for (int j = 0; j < 8; j++) {
                if (getbit(bitmap[i], j) == 0) {
                    return i * 8 + j;
                }
            }
        }
        return -1; // 没有空闲块
    }

    // 分配物理块
    int allocate_frame() {
        int frame = find_free_block(bitmap, MEM_SIZE / 8);
        if (frame != -1 && frame < MEM_SIZE) {
            setbit(&bitmap[frame / 8], frame % 8, 1);
            return frame;
        }
        return -1;
    }

    // 释放物理块
    void free_frame(int frame) {
        if (frame >= 0 && frame < MEM_SIZE) {
            setbit(&bitmap[frame / 8], frame % 8, 0);
        }
    }

    // 分配置换空间块
    int allocate_swap_block() {
        int block = find_free_block(swap_bitmap, swap_size / 8);
        if (block != -1 && block < swap_size) {
            setbit(&swap_bitmap[block / 8], block % 8, 1);
            return block;
        }
        return -1;
    }

    // 释放置换空间块
    void free_swap_block(int block) {
        if (block >= 0 && block < swap_size) {
            setbit(&swap_bitmap[block / 8], block % 8, 0);
        }
    }

    // FIFO页面置换（仅从当前进程的已分配帧中选择）
    int fifo_replace_current() {
        if (!current_process || current_process->fifo_queue.empty())
            return -1;

        // 找到第一个有效的页面进行置换
        while (!current_process->fifo_queue.empty()) {
            int victim_frame = current_process->fifo_queue.front();
            current_process->fifo_queue.pop();

            // 检查这个帧是否仍然属于当前进程且有效
            for (int i = 0; i < current_process->page_table.size(); i++) {
                if (current_process->page_table[i].valid &&
                    current_process->page_table[i].frame_number == victim_frame) {

                    // 如果页面被修改过，需要写回置换空间
                    if (current_process->page_table[i].dirty) {
                        int swap_block = allocate_swap_block();
                        if (swap_block != -1) {
                            cout << "将进程" << current_process->name
                                << "的内存块" << victim_frame
                                << "内容写入置换空间块" << swap_block << endl;
                        }
                    }

                    current_process->page_table[i].valid = false;
                    current_process->page_table[i].frame_number = -1;
                    return victim_frame;
                }
            }
        }
        return -1;
    }

    // LRU页面置换（仅从当前进程的已分配帧中选择）
    int lru_replace_current() {
        if (!current_process) return -1;

        int lru_frame = -1;
        int lru_page = -1;
        int min_access_time = INT_MAX;

        // 在当前进程的所有有效页面中找最近最久未使用的
        for (int i = 0; i < current_process->page_table.size(); i++) {
            if (current_process->page_table[i].valid &&
                current_process->page_table[i].access_time < min_access_time) {
                min_access_time = current_process->page_table[i].access_time;
                lru_frame = current_process->page_table[i].frame_number;
                lru_page = i;
            }
        }

        if (lru_frame != -1 && lru_page != -1) {
            // 如果页面被修改过，需要写回置换空间
            if (current_process->page_table[lru_page].dirty) {
                int swap_block = allocate_swap_block();
                if (swap_block != -1) {
                    cout << "将进程" << current_process->name
                        << "的内存块" << lru_frame
                        << "内容写入置换空间块" << swap_block << endl;
                }
            }

            current_process->page_table[lru_page].valid = false;
            current_process->page_table[lru_page].frame_number = -1;
        }

        return lru_frame;
    }

public:
    PagingMemoryManager() : time_counter(0), current_process(nullptr) {
        swap_size = MEM_SIZE * 2; // 置换空间为内存的2倍

        // 初始化位示图
        srand(time(NULL));
        for (int i = 0; i < MEM_SIZE / 8; i++) {
            bitmap[i] = static_cast<char>(rand());
        }

        for (int i = 0; i < swap_size / 8; i++) {
            swap_bitmap[i] = 0; // 置换空间初始为空
        }
    }

    // 创建进程
    void create_process() {
        string name;
        int size;

        cout << "输入进程名: ";
        cin >> name;
        cout << "输入进程大小(字节): ";
        cin >> size;

        PCB* pcb = new PCB(name, size);
        processes.push_back(pcb);

        // 分配页表空间
        cout << "进程" << name << "需要" << pcb->page_count << "个页面" << endl;

        // 询问初始装入页数
        int initial_pages;
        cout << "输入初始装入内存的页数(0-" << pcb->page_count << "): ";
        cin >> initial_pages;

        if (initial_pages > pcb->page_count) {
            initial_pages = pcb->page_count;
        }

        // 分配初始页面
        int allocated_count = 0;
        for (int i = 0; i < initial_pages; i++) {
            int frame = allocate_frame();
            if (frame != -1) {
                pcb->page_table[i].frame_number = frame;
                pcb->page_table[i].valid = true;
                pcb->page_table[i].load_time = time_counter;
                pcb->page_table[i].access_time = time_counter;

                // 记录分配给该进程的帧
                pcb->allocated_frames.insert(frame);
                pcb->fifo_queue.push(frame);

                cout << "页面" << i << "分配到内存块" << frame << endl;
                allocated_count++;
            }
            else {
                cout << "内存不足，页面" << i << "无法装入内存" << endl;
                // 分配到置换空间
                int swap_block = allocate_swap_block();
                if (swap_block != -1) {
                    cout << "页面" << i << "分配到置换空间块" << swap_block << endl;
                }
            }
        }

        cout << "进程" << name << "成功分配了" << allocated_count << "个内存帧" << endl;

        // 剩余页面分配到置换空间
        for (int i = allocated_count; i < pcb->page_count; i++) {
            int swap_block = allocate_swap_block();
            if (swap_block != -1) {
                cout << "页面" << i << "分配到置换空间块" << swap_block << endl;
            }
        }

        current_process = pcb;
        time_counter++;
    }

    // 地址转换
    void address_translation() {
        if (!current_process) {
            cout << "没有当前进程！" << endl;
            return;
        }

        int logical_address;
        cout << "输入逻辑地址(<0退出): ";
        cin >> logical_address;

        if (logical_address < 0) return;

        if (logical_address >= current_process->size) {
            cout << "逻辑地址越界！" << endl;
            return;
        }

        // 计算页号和页内偏移
        int page_size_bits = mylog2(BLOCK_SIZE);
        int page_number = logical_address >> page_size_bits;
        int offset = logical_address & ((1 << page_size_bits) - 1);

        cout << "逻辑地址" << logical_address << "对应的页号为:" << page_number
            << ", 页内偏移地址为:" << offset << endl;

        if (page_number >= current_process->page_count) {
            cout << "页号越界！" << endl;
            return;
        }

        // 检查页面是否在内存中
        if (!current_process->page_table[page_number].valid) {
            cout << "页面" << page_number << "不在内存中，需要页面置换..." << endl;

            // 检查当前进程是否还有可用的内存帧
            if (current_process->allocated_frames.empty()) {
                cout << "当前进程没有可用的内存帧！" << endl;
                return;
            }

            // 选择置换算法
            int algorithm;
            cout << "选择置换算法 (1-FIFO, 2-LRU): ";
            cin >> algorithm;

            int free_frame = -1;

            // 从当前进程的已分配帧中找一个可用的
            if (algorithm == 1) {
                free_frame = fifo_replace_current();
            }
            else {
                free_frame = lru_replace_current();
            }

            if (free_frame == -1) {
                cout << "置换失败！无法找到可置换的页面" << endl;
                return;
            }

            cout << "使用" << (algorithm == 1 ? "FIFO" : "LRU")
                << "算法选中进程专属内存块" << free_frame << "进行置换" << endl;

            // 将页面装入内存
            current_process->page_table[page_number].frame_number = free_frame;
            current_process->page_table[page_number].valid = true;
            current_process->page_table[page_number].load_time = time_counter;
            current_process->page_table[page_number].access_time = time_counter;

            // 更新FIFO队列
            current_process->fifo_queue.push(free_frame);

            cout << "将页面" << page_number << "调入进程专属内存块" << free_frame << "中" << endl;
        }

        // 更新访问时间
        current_process->page_table[page_number].access_time = time_counter;

        // 询问是否为写操作
        char write_op;
        cout << "是否为写指令(Y/N): ";
        cin >> write_op;
        if (write_op == 'Y' || write_op == 'y') {
            current_process->page_table[page_number].dirty = true;
        }

        // 计算物理地址
        int physical_address = current_process->page_table[page_number].frame_number * BLOCK_SIZE + offset;
        cout << "逻辑地址" << logical_address << "对应的物理地址为:" << physical_address << endl;

        // 将原地址转换为空地址（重置为0）
        logical_address = 0;
        cout << "原逻辑地址已转换为空地址: " << logical_address << endl;

        time_counter++;
    }

    // 显示位示图
    void show_bitmap() {
        cout << "内存位示图:" << endl;
        for (int i = 0; i < MEM_SIZE / 8; i++) {
            cout << "字节" << i << ": ";
            for (int j = 0; j < 8; j++) {
                cout << getbit(bitmap[i], j) << " ";
            }
            cout << endl;
        }

        cout << "\n置换空间位示图:" << endl;
        for (int i = 0; i < swap_size / 8; i++) {
            cout << "字节" << i << ": ";
            for (int j = 0; j < 8; j++) {
                cout << getbit(swap_bitmap[i], j) << " ";
            }
            cout << endl;
        }
    }

    // 显示进程页表
    void show_page_table() {
        if (!current_process) {
            cout << "没有当前进程！" << endl;
            return;
        }

        cout << "进程" << current_process->name << "的页表:" << endl;
        cout << "页号\t块号\t有效位\t修改位\t加载时间\t访问时间" << endl;
        for (int i = 0; i < current_process->page_table.size(); i++) {
            cout << i << "\t"
                << current_process->page_table[i].frame_number << "\t"
                << current_process->page_table[i].valid << "\t"
                << current_process->page_table[i].dirty << "\t"
                << current_process->page_table[i].load_time << "\t\t"
                << current_process->page_table[i].access_time << endl;
        }

        // 显示进程专属内存帧
        cout << "进程" << current_process->name << "专属内存帧: ";
        for (int frame : current_process->allocated_frames) {
            cout << frame << " ";
        }
        cout << endl;
    }

    // 终止进程
    void terminate_process() {
        if (!current_process) {
            cout << "没有当前进程！" << endl;
            return;
        }

        // 释放该进程占用的所有内存块
        for (int frame : current_process->allocated_frames) {
            free_frame(frame);
        }

        // 从进程列表中移除
        auto it = find(processes.begin(), processes.end(), current_process);
        if (it != processes.end()) {
            processes.erase(it);
        }

        delete current_process;
        current_process = nullptr;
        cout << "进程已终止，所有专属内存帧已释放" << endl;
    }

    // 显示菜单
    void show_menu() {
        cout << "\n=== 分页式存储管理系统 ===" << endl;
        cout << "1. 创建进程" << endl;
        cout << "2. 地址转换" << endl;
        cout << "3. 显示位示图" << endl;
        cout << "4. 显示页表" << endl;
        cout << "5. 终止当前进程" << endl;
        cout << "6. 切换当前进程" << endl;
        cout << "0. 退出" << endl;
        cout << "选择操作: ";
    }

    // 切换当前进程
    void switch_process() {
        if (processes.empty()) {
            cout << "没有进程！" << endl;
            return;
        }

        cout << "可用进程:" << endl;
        for (int i = 0; i < processes.size(); i++) {
            cout << i << ". " << processes[i]->name
                << " (内存帧:" << processes[i]->allocated_frames.size()
                << "个)" << endl;
        }

        int choice;
        cout << "选择进程编号: ";
        cin >> choice;

        if (choice >= 0 && choice < processes.size()) {
            current_process = processes[choice];
            cout << "切换到进程: " << current_process->name << endl;
        }
        else {
            cout << "无效选择！" << endl;
        }
    }

    // 运行系统
    void run() {
        int choice;
        do {
            show_menu();
            cin >> choice;

            switch (choice) {
            case 1:
                create_process();
                break;
            case 2:
                address_translation();
                break;
            case 3:
                show_bitmap();
                break;
            case 4:
                show_page_table();
                break;
            case 5:
                terminate_process();
                break;
            case 6:
                switch_process();
                break;
            case 0:
                cout << "系统退出" << endl;
                break;
            default:
                cout << "无效选择！" << endl;
            }
        } while (choice != 0);
    }
};

int main() {
    PagingMemoryManager manager;
    manager.run();
    return 0;
}
