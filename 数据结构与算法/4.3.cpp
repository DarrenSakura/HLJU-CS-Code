#include <iostream>

using namespace std;

// 定义循环链表节点
struct Node {
    int data;
    Node* next;
};

// 初始化循环链表队列（带头结点）
void initQueue(Node*& rear) {
    rear = new Node;  // 创建头结点
    rear->next = rear;  // 头结点指向自己，表示空队列
}

// 判断队列是否为空
bool isEmpty(Node* rear) {
    return rear->next == rear;
}

// 置队空：删除队列中的所有元素
void clearQueue(Node*& rear) {
    while (!isEmpty(rear)) {
        Node* front = rear->next->next;  // 队头元素
        delete rear->next;  // 删除队头元素
        rear->next = front;  // 更新头结点指向下一个节点
    }
    cout << "队列已置空！" << endl;
}

// 入队操作：在队尾插入新元素
void enQueue(Node*& rear, int x) {
    Node* newNode = new Node;  // 创建新节点
    newNode->data = x;

    // 将新节点插入到尾部
    newNode->next = rear->next;  // 新节点的next指向头结点
    rear->next = newNode;  // 原尾节点的next指向新节点
    rear = newNode;  // 更新尾指针，指向新节点

    cout << "入队元素: " << x << endl;
}

// 出队操作：删除队头元素
bool deQueue(Node*& rear, int& x) {
    if (isEmpty(rear)) {
        cout << "队列为空，无法出队！" << endl;
        return false;  // 队列为空，出队失败
    }

    Node* front = rear->next->next;  // 队头元素
    x = front->data;  // 获取队头数据

    if (front == rear) {
        // 队列中只有一个元素，出队后队列为空
        rear->next = rear;  // 头结点指向自己
    } else {
        rear->next->next = front->next;  // 头结点指向下一个节点
    }

    delete front;  // 释放队头元素
    cout << "出队元素: " << x << endl;
    return true;  // 出队成功
}

// 显示队列中的所有元素
void displayQueue(Node* rear) {
    if (isEmpty(rear)) {
        cout << "队列为空！" << endl;
        return;
    }

    cout << "队列中的元素: ";
    Node* temp = rear->next->next;  // 从队头开始遍历
    while (temp != rear->next) {  // 遍历到尾部节点
        cout << temp->data << " ";
        temp = temp->next;
    }
    cout << endl;
}

// 测试代码
int main() {
    Node* rear;  // 定义队尾指针
    initQueue(rear);  // 初始化队列

    // 显示初始队列状态
    displayQueue(rear);

    // 测试入队操作
    enQueue(rear, 10);
    displayQueue(rear);

    enQueue(rear, 20);
    displayQueue(rear);

    enQueue(rear, 30);
    displayQueue(rear);

    // 测试出队操作
    int x;
    if (deQueue(rear, x)) {
        displayQueue(rear);
    }

    if (deQueue(rear, x)) {
        displayQueue(rear);
    }

    // 测试置队空操作
    clearQueue(rear);
    displayQueue(rear);

    return 0;
}
