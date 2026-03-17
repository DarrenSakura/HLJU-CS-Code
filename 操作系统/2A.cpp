#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <cmath>

using namespace std;

// 进程状态枚举
enum State {
    READY,
    RUN_CPU,
    RUN_IO,
    WAITING,
    DONE
};

// 指令结构体
struct Instruction {
    string type;    // "cpu" 或 "io"
    uint32_t vaddr; // 虚拟地址
};

// 页表项结构体
struct PTE {
    int frameNum; // 物理页帧号，-1表示无效
    bool valid;   // 有效位
    
    PTE() : frameNum(-1), valid(false) {}
};

// 进程控制块 PCB
struct PCB {
    int pid;
    string name;
    State state;
    vector<Instruction> instructions;
    int pc;             // 程序计数器
    int wait_timer;     // I/O 等待计时器
    vector<PTE> pageTable; // 页表
    vector<int> validPagesS; // 进程的有效页集合S (用于验证和生成程序)
    string current_run_str; // 用于保存当前执行指令的输出字符串

    PCB(int id, int numVirtualPages) : pid(id), state(READY), pc(0), wait_timer(0) {
        name = "P" + to_string(id);
        pageTable.resize(numVirtualPages);
    }
};

// 全局系统参数
int param_x, param_a, param_b;
int min_size, max_size;
double p_valid, p_available;
int numVirtualPages, numPhysicalFrames;
vector<int> bitmap_arr;

// 格式化输出十六进制数
string toHexString(uint32_t val, int width) {
    stringstream ss;
    ss << uppercase << hex << setfill('0') << setw(width) << val;
    return ss.str();
}

// 任务1扩展：生成带有虚拟地址引用的随机程序文件和有效页集合
void generateProgramsExt(int k, int percent, int min_inst, int max_inst) {
    srand(time(NULL));
    cout << "--- 正在生成 " << k << " 个程序文件及内存布局 ---" << endl;
    
    int hex_width = (param_a + 3) / 4; // 计算输出16进制需要的字符数
    
    for (int i = 0; i < k; ++i) {
        string filename = "P" + to_string(i) + ".txt";
        ofstream outFile(filename);
        
        // 随机生成进程大小 y
        int y = min_size + rand() % (max_size - min_size + 1);
        
        // 随机生成有效页集合 S
        vector<int> all_vpages(numVirtualPages);
        for(int j=0; j<numVirtualPages; ++j) all_vpages[j] = j;
        random_shuffle(all_vpages.begin(), all_vpages.end());
        vector<int> S(all_vpages.begin(), all_vpages.begin() + y);
        
        // 生成指令序列
        int num_inst = min_inst + rand() % (max_inst - min_inst + 1);
        outFile << y << " "; // 文件第一行或开头保存进程有效页数
        for(int p : S) outFile << p << " "; // 保存有效页集合S
        outFile << endl;

        for (int j = 0; j < num_inst; ++j) {
            string type = (rand() % 100 + 1 <= percent) ? "cpu" : "io";
            uint32_t vaddr = 0;
            
            double r_prob = (double)rand() / RAND_MAX;
            if (r_prob <= p_valid) {
                // 生成合法虚拟地址
                int page = S[rand() % S.size()];
                int offset = rand() % (1 << param_x);
                vaddr = (page << param_x) | offset;
            } else {
                // 生成非法虚拟地址
                vector<int> invalid_pages;
                for(int p=0; p<numVirtualPages; ++p) {
                    if (find(S.begin(), S.end(), p) == S.end()) {
                        invalid_pages.push_back(p);
                    }
                }
                if (!invalid_pages.empty()) {
                    int page = invalid_pages[rand() % invalid_pages.size()];
                    int offset = rand() % (1 << param_x);
                    vaddr = (page << param_x) | offset;
                }
            }
            outFile << type << " " << toHexString(vaddr, hex_width) << "\n";
        }
        outFile.close();
    }
}

// 初始化位示图
void initBitmap() {
    bitmap_arr.assign(numPhysicalFrames, 0);
    for (int i = 0; i < numPhysicalFrames; ++i) {
        double r = (double)rand() / RAND_MAX;
        if (r > p_available) { // p_available是0的概率，1-p_available是1的概率
            bitmap_arr[i] = 1;
        }
    }
}

// 打印位示图
void printBitmap(const string& title) {
    cout << title << endl;
    for (int i = 0; i < numPhysicalFrames; ++i) {
        cout << bitmap_arr[i];
    }
    cout << endl;
}

// 为进程分配物理页
void allocateMemory(PCB& pcb) {
    for (int vpage : pcb.validPagesS) {
        // 在位示图中找一个空闲帧
        int allocated_frame = -1;
        for (int i = 0; i < numPhysicalFrames; ++i) {
            if (bitmap_arr[i] == 0) {
                bitmap_arr[i] = 1;
                allocated_frame = i;
                break;
            }
        }
        if (allocated_frame != -1) {
            pcb.pageTable[vpage].valid = true;
            pcb.pageTable[vpage].frameNum = allocated_frame;
        } else {
            cout << "内存分配失败，物理内存不足！" << endl;
            // 简单处理：实际操作系统中会涉及页面置换，此处直接忽略
        }
    }
}

// 回收进程内存
void freeMemory(PCB& pcb) {
    for (int i = 0; i < numVirtualPages; ++i) {
        if (pcb.pageTable[i].valid) {
            int frame = pcb.pageTable[i].frameNum;
            bitmap_arr[frame] = 0; // 回收
            pcb.pageTable[i].valid = false;
            pcb.pageTable[i].frameNum = -1;
        }
    }
}

// 打印页表
void printPageTables(vector<PCB>& processes) {
    for (const auto& pcb : processes) {
        cout << "pageTable of " << pcb.name << endl;
        for (int i = 0; i < numVirtualPages; ++i) {
            cout << left << i << ":";
            if (pcb.pageTable[i].valid) cout << pcb.pageTable[i].frameNum;
            else cout << "-";
            cout << "   ";
            if ((i + 1) % 10 == 0) cout << endl; // 每行打印10个页表项
        }
        cout << endl;
    }
}

// 任务2扩展：模拟调度和地址转换
void simulate(int k) {
    vector<PCB> processes;
    queue<int> ready_queue;
    
    int hex_v_width = (param_a + 3) / 4;
    int hex_p_width = (param_b + 3) / 4;

    for (int i = 0; i < k; ++i) {
        PCB pcb(i, numVirtualPages);
        string filename = "P" + to_string(i) + ".txt";
        ifstream inFile(filename);
        
        int y;
        inFile >> y;
        for (int j = 0; j < y; ++j) {
            int vpage;
            inFile >> vpage;
            pcb.validPagesS.push_back(vpage);
        }
        
        string type, vaddr_str;
        while (inFile >> type >> vaddr_str) {
            Instruction inst;
            inst.type = type;
            inst.vaddr = stoul(vaddr_str, nullptr, 16);
            pcb.instructions.push_back(inst);
        }
        inFile.close();
        
        allocateMemory(pcb);
        processes.push_back(pcb);
        ready_queue.push(i);
    }

    printBitmap("after loading process");
    cout << endl;
    printPageTables(processes);

    int time = 1;
    int cpu_busy_time = 0;
    int io_busy_time = 0;
    int running_pid = -1;
    
    cout << left << setw(6) << "Time";
    for (int i = 0; i < k; ++i) cout << left << setw(28) << ("PID:" + to_string(i));
    cout << left << setw(6) << "CPU" << "IOs" << endl;

    bool all_done = false;

    while (!all_done) {
        bool cpu_busy = false;
        int active_ios = 0;
        bool state_changed = false;

        // 1. 更新 WAITING 进程
        for (int i = 0; i < k; ++i) {
            if (processes[i].state == WAITING) {
                processes[i].wait_timer--;
                if (processes[i].wait_timer == 0) {
                    processes[i].pc++;
                    if (processes[i].pc >= processes[i].instructions.size()) {
                        processes[i].state = DONE;
                        freeMemory(processes[i]);
                    } else {
                        processes[i].state = READY;
                        ready_queue.push(i);
                    }
                    state_changed = true;
                }
            }
        }

        // 2. 更新 RUN 进程
        if (running_pid != -1) {
            PCB& cur_p = processes[running_pid];
            if (cur_p.state == RUN_CPU) {
                cur_p.pc++;
                if (cur_p.pc >= cur_p.instructions.size()) {
                    cur_p.state = DONE;
                    freeMemory(cur_p);
                    running_pid = -1;
                } else {
                    cur_p.state = READY;
                    ready_queue.push(running_pid);
                    running_pid = -1;
                }
            } else if (cur_p.state == RUN_IO) {
                cur_p.state = WAITING;
                cur_p.wait_timer = 4;
                running_pid = -1;
            }
        }

        // 3. 调度新进程
        if (running_pid == -1 && !ready_queue.empty()) {
            running_pid = ready_queue.front();
            ready_queue.pop();
            PCB& cur_p = processes[running_pid];
            
            Instruction inst = cur_p.instructions[cur_p.pc];
            uint32_t vaddr = inst.vaddr;
            uint32_t pageNum = vaddr >> param_x;
            uint32_t offset = vaddr & ((1 << param_x) - 1);

            stringstream out_ss;
            
            // 地址转换逻辑
            if (pageNum >= numVirtualPages || !cur_p.pageTable[pageNum].valid) {
                // 处理机异常，越界或缺页
                out_ss << "RUN:" << inst.type << " " << toHexString(vaddr, hex_v_width) << " error";
                cur_p.state = DONE;
                cur_p.current_run_str = out_ss.str();
                freeMemory(cur_p); // 进程异常终止，回收内存
                // 不将 running_pid 设为 -1，让他占用当前时钟周期显示 error
            } else {
                // 转换成功
                uint32_t paddr = (cur_p.pageTable[pageNum].frameNum << param_x) | offset;
                out_ss << "RUN:" << inst.type << " [" << toHexString(vaddr, hex_v_width) << ";" << toHexString(paddr, hex_p_width) << "]";
                cur_p.current_run_str = out_ss.str();
                
                if (inst.type == "cpu") cur_p.state = RUN_CPU;
                else cur_p.state = RUN_IO;
            }
        }

        // 4. 统计
        all_done = true;
        for (int i = 0; i < k; ++i) {
            if (processes[i].state != DONE) all_done = false;
            if (processes[i].state == WAITING) active_ios++;
        }
        
        if (all_done && running_pid == -1) break;

        if (running_pid != -1) {
            cpu_busy = true;
            cpu_busy_time++;
        }
        if (active_ios > 0) io_busy_time++;

        // 5. 打印状态
        string time_str = to_string(time) + (state_changed ? "*" : "");
        cout << left << setw(6) << time_str;
        
        for (int i = 0; i < k; ++i) {
            string p_state_str;
            if (processes[i].state == RUN_CPU || processes[i].state == RUN_IO || 
               (processes[i].state == DONE && processes[i].current_run_str.find("error") != string::npos && i == running_pid)) {
                p_state_str = processes[i].current_run_str;
            } else if (processes[i].state == READY) p_state_str = "READY";
            else if (processes[i].state == WAITING) p_state_str = "WAITING";
            else p_state_str = "DONE";
            
            cout << left << setw(28) << p_state_str;
        }
        
        cout << left << setw(6) << (cpu_busy ? "1" : "");
        cout << (active_ios > 0 ? to_string(active_ios) : "") << endl;
        
        // 异常结束的进程清理状态为普通DONE
        if (running_pid != -1 && processes[running_pid].state == DONE) {
             running_pid = -1; 
        }

        time++;
    }

    int total_time = time - 1;
    cout << "\nStatus: Total Time " << total_time << endl;
    cout << fixed << setprecision(2);
    cout << "Status: CPU Busy " << cpu_busy_time << " (" << ((double)cpu_busy_time / total_time) * 100 << ")" << endl;
    cout << "Status: IO Busy  " << io_busy_time << " (" << ((double)io_busy_time / total_time) * 100 << ")" << endl << endl;
    
    printBitmap("bitmap when completed");
}

int main() {
    int k, percent, min_inst, max_inst;
    
    // 初始化系统参数
    param_x = 12; // 页内偏移
    param_a = 16; // 虚址位数
    param_b = 18; // 物址位数
    min_size = 3; // 最小页数
    max_size = 7; // 最大页数
    p_valid = 0.8;
    p_available = 0.85;

    numVirtualPages = 1 << (param_a - param_x); // 16个虚页
    numPhysicalFrames = 1 << (param_b - param_x); // 64个物理页
    
    initBitmap();
    printBitmap("initial bitmap");
    
    cout << "请输入: 程序个数k, CPU指令占比percent, 最小指令数min, 最大指令数max\n";
    if (cin >> k >> percent >> min_inst >> max_inst) {
        generateProgramsExt(k, percent, min_inst, max_inst);
        simulate(k);
    }
    return 0;
}