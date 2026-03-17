#include <iostream>

// 定义链表节点结构
struct Node {
    int data;
    Node* next;
};

// 删除节点 s_value 的前驱节点
void deletePrevNode(Node*& head, int s_value) {
    if (!head || head->next == head) {
        // 链表为空或者只有一个节点时，无法删除前驱节点
        return;
    }

    Node* curr = head;
    Node* prev = nullptr;
    Node* before_prev = nullptr;

    // 如果链表只有两个节点时，且输入为head节点值
    if (head->next->next == head && head->data == s_value) {
        Node* toDelete = head->next;
        head->next = head;
        delete toDelete;
        return;
    }

    // 处理链表头节点的特殊情况
    if (head->data == s_value) {
        // 找到尾节点并删除尾节点
        Node* tail = head;
        while (tail->next != head) {
            tail = tail->next;
        }
        Node* toDelete = tail;
        tail->next = head->next;
        head = head->next;  // 更新 head 指针
        delete toDelete;
        return;
    }

    // 遍历链表，找到 s_value 和它的前驱节点以及前驱节点的前驱节点
    do {
        before_prev = prev;
        prev = curr;
        curr = curr->next;
    } while (curr != head && curr->data != s_value);

    // 如果找到了 s_value，删除前驱节点 prev
    if (curr->data == s_value) {
        if (before_prev) {
            before_prev->next = curr;
            delete prev;
        }
    }
}

// 打印链表内容
void printList(Node* head) {
    if (!head) return;

    Node* temp = head;
    do {
        std::cout << temp->data << " ";
        temp = temp->next;
    } while (temp != head);
    std::cout << std::endl;
}

int main() {
    // 创建一个单循环链表 1 -> 2 -> 3 -> 4 -> 5 -> 1
    Node* head = new Node{1, nullptr};
    Node* second = new Node{2, nullptr};
    Node* third = new Node{3, nullptr};
    Node* fourth = new Node{4, nullptr};
    Node* fifth = new Node{5, nullptr};

    head->next = second;
    second->next = third;
    third->next = fourth;
    fourth->next = fifth;
    fifth->next = head;  // 成为循环链表

    std::cout << "原链表 ";
    printList(head);

    // 用户输入要删除前驱节点的节点值
    int s_value;
    std::cout << "输入要删除前驱节点的节点 ";
    std::cin >> s_value;

    // 删除 s_value 节点的前驱节点
    deletePrevNode(head, s_value);

    std::cout << "删除后 " << s_value << ": ";
    printList(head);

    return 0;
}
