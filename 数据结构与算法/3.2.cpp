#include <iostream>
#include <cctype>  // 用于判断字符类型

// 定义链表节点结构
struct Node {
    char data;
    Node* next;
};

// 初始化链表头节点
Node* initList() {
    Node* head = new Node;
    head->next = head;  // 循环链表
    return head;
}

// 将节点插入到循环链表的尾部
void insertToList(Node* head, Node* node) {
    Node* tail = head;
    // 找到当前链表的尾部
    while (tail->next != head) {
        tail = tail->next;
    }
    // 插入节点
    tail->next = node;
    node->next = head;  // 形成循环链
}

// 打印循环链表
void printList(Node* head) {
    if (head->next == head) {
        std::cout << "Empty list" << std::endl;
        return;
    }

    Node* temp = head->next;  // 从头节点后面的第一个节点开始打印
    while (temp != head) {
        std::cout << temp->data << " ";
        temp = temp->next;
    }
    std::cout << std::endl;
}

// 将单链表分成三个不同类别的循环链表
void splitList(Node* original, Node* letters, Node* digits, Node* others) {
    Node* current = original->next;  // 跳过原链表的头结点

    while (current) {
        Node* next = current->next;  // 暂存下一个节点，防止链断开
        current->next = nullptr;     // 断开当前节点的 next，方便插入
        if (isalpha(current->data)) {
            insertToList(letters, current);  // 插入到字母链表
        } else if (isdigit(current->data)) {
            insertToList(digits, current);   // 插入到数字链表
        } else {
            insertToList(others, current);   // 插入到其它字符链表
        }
        current = next;
    }
}

int main() {
    // 创建原始单链表 头结点
    Node* original = new Node;
    original->next = nullptr;

    // 创建几个测试节点
    Node* n1 = new Node{'a', nullptr};
    Node* n2 = new Node{'1', nullptr};
    Node* n3 = new Node{'#', nullptr};
    Node* n4 = new Node{'b', nullptr};
    Node* n5 = new Node{'2', nullptr};
    Node* n6 = new Node{'$', nullptr};

    // 构建原始单链表: a -> 1 -> # -> b -> 2 -> $
    original->next = n1;
    n1->next = n2;
    n2->next = n3;
    n3->next = n4;
    n4->next = n5;
    n5->next = n6;

    // 初始化三个循环链表
    Node* letters = initList();
    Node* digits = initList();
    Node* others = initList();

    // 分离原始链表到不同类别的链表中
    splitList(original, letters, digits, others);

    // 打印分离后的链表
    std::cout << "字母: ";
    printList(letters);

    std::cout << "数字: ";
    printList(digits);

    std::cout << "其他: ";
    printList(others);

    return 0;
}
