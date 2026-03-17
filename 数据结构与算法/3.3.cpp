#include <iostream>

// 定义双向链表的节点结构
struct Node {
    int data;     // 数据域
    int freq;     // 访问频度域
    Node* prior;  // 前驱指针
    Node* next;   // 后继指针

    // 构造函数
    Node(int d) : data(d), freq(0), prior(nullptr), next(nullptr) {}
};

// 双向链表的 LOCATE 函数
Node* LOCATE(Node*& head, int x) {
    Node* current = head;

    // 遍历链表查找值为 x 的节点
    while (current != nullptr && current->data != x) {
        current = current->next;
    }

    // 如果未找到值为 x 的节点，返回 nullptr
    if (!current) {
        return nullptr;
    }

    // 找到节点后，访问频度加 1
    current->freq++;

    // 需要将当前节点按 freq 值调整顺序
    // 如果当前节点是第一个节点或已经在正确位置，则不需要调整
    if (current == head || current->prior == nullptr || current->freq <= current->prior->freq) {
        return current;
    }

    // 将当前节点从链表中移除
    if (current->next != nullptr) {
        current->next->prior = current->prior;
    }
    if (current->prior != nullptr) {
        current->prior->next = current->next;
    }

    // 将 current 插入到合适的位置
    Node* temp = current->prior;  // 从当前节点之前的节点开始寻找插入位置

    // 向前寻找 freq 大于等于 current->freq 的节点
    while (temp != nullptr && current->freq > temp->freq) {
        temp = temp->prior;
    }

    // temp 是比 current->freq 小的节点（或为空）
    if (temp == nullptr) {
        // 插入到头结点位置
        current->next = head;
        head->prior = current;
        current->prior = nullptr;
        head = current;
    } else {
        // 插入到 temp 节点之后
        current->next = temp->next;
        current->prior = temp;
        if (temp->next != nullptr) {
            temp->next->prior = current;
        }
        temp->next = current;
    }

    return current;
}

// 打印链表内容
void printList(Node* head) {
    Node* current = head;
    while (current != nullptr) {
        std::cout << "Data: " << current->data << ", Freq: " << current->freq << std::endl;
        current = current->next;
    }
}

int main() {
    // 构建双向链表：1 <-> 2 <-> 3 <-> 4
    Node* head = new Node(1);
    Node* second = new Node(2);
    Node* third = new Node(3);
    Node* fourth = new Node(4);

    head->next = second;
    second->prior = head;
    second->next = third;
    third->prior = second;
    third->next = fourth;
    fourth->prior = third;

    // 测试 LOCATE 函数
    std::cout << "LOCATE运算之前:" << std::endl;
    printList(head);

    LOCATE(head, 3); // 访问节点 3
    LOCATE(head, 1); // 访问节点 1
    LOCATE(head, 3); // 再次访问节点 3
    LOCATE(head, 4); // 访问节点 4
    LOCATE(head, 3); // 再次访问节点 3

    std::cout << "\nLOCATE运算之后:" << std::endl;
    printList(head);

    return 0;
}
