#include <iostream>

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

// 头插法，将节点插入链表头部
void InsertToHead(ListNode*& head, ListNode* node) {
    node->next = head;
    head = node;
}

// 将两个递增有序链表 A 和 B 归并成递减有序链表 C
ListNode* MergeDescending(ListNode* A, ListNode* B) {
    ListNode* C = nullptr; // 用于存储结果链表的头指针

    // 遍历链表 A 和 B，将较大的元素插入 C 的头部
    while (A != nullptr && B != nullptr) {
        if (A->val > B->val) {
            ListNode* temp = A;
            A = A->next;
            InsertToHead(C, temp); // 头插法
        } else {
            ListNode* temp = B;
            B = B->next;
            InsertToHead(C, temp); // 头插法
        }
    }

    // 将剩余的元素插入 C
    while (A != nullptr) {
        ListNode* temp = A;
        A = A->next;
        InsertToHead(C, temp); // 头插法
    }

    while (B != nullptr) {
        ListNode* temp = B;
        B = B->next;
        InsertToHead(C, temp); // 头插法
    }

    return C; // 返回合并后的链表 C
}

// 打印链表
void PrintList(ListNode* head) {
    while (head != nullptr) {
        std::cout << head->val << " ";
        head = head->next;
    }
    std::cout << std::endl;
}

// 创建链表（递增顺序）
ListNode* CreateList(const std::initializer_list<int>& vals) {
    ListNode* head = nullptr;
    ListNode* tail = nullptr;
    for (int val : vals) {
        ListNode* node = new ListNode(val);
        if (head == nullptr) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }
    return head;
}

int main() {
    // 创建两个递增有序的链表 A 和 B
    ListNode* A = CreateList({1, 3, 5, 7});
    ListNode* B = CreateList({2, 4, 6, 8});

    // 归并 A 和 B 到 C，并按递减顺序排列
    ListNode* C = MergeDescending(A, B);

    // 打印结果链表 C
    PrintList(C);

    return 0;
}
