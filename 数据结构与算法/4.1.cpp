#include <iostream>
#include <stack>

using namespace std;

// 定义单链表的节点结构
struct ListNode {
    char val;
    ListNode* next;
    ListNode(char x) : val(x), next(nullptr) {}
};

// 判断字符串是否中心对称
bool isPalindrome(ListNode* head) {
    stack<char> s;
    ListNode* temp = head;

    // 将链表的字符存入栈中
    while (temp != nullptr) {
        s.push(temp->val);
        temp = temp->next;
    }

    // 再次遍历链表，和栈中的字符比较
    temp = head;
    while (temp != nullptr) {
        if (s.top() != temp->val) {
            return false;
        }
        s.pop();//移除栈顶元素
        temp = temp->next;
    }

    return true;
}

// 测试代码
int main() {
    // 创建一个单链表: x -> y -> z -> z -> y -> x
    ListNode* head = new ListNode('x');
    head->next = new ListNode('y');
    head->next->next = new ListNode('z');
    head->next->next->next = new ListNode('z');
    head->next->next->next->next = new ListNode('y');
    head->next->next->next->next->next = new ListNode('x');
    if (isPalindrome(head)) {
        cout << "字符串是中心对称的。" << endl;
    } else {
        cout << "字符串不是中心对称的。" << endl;
    }

    return 0;
}
