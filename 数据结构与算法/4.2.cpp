#include <iostream>
#include <stack>
#include <string>

using namespace std;

// 判断算术表达式中的圆括号是否配对
bool isParenthesesMatched(const string& expression) {
    stack<char> s;  // 用栈存储左括号

    // 遍历表达式中的每个字符
    for (char ch : expression) {
        if (ch == '(') {
            // 遇到左括号，入栈
            s.push(ch);
        } else if (ch == ')') {
            // 遇到右括号，检查栈顶是否有对应的左括号
            if (s.empty() || s.top() != '(') {
                return false;  // 不匹配
            }
            s.pop();  // 弹出与之匹配的左括号
        }
    }

    // 如果栈为空，说明所有括号匹配成功
    return s.empty();
}

int main() {
    string expression;

    // 输入一个算术表达式
    cout << "请输入一个算术表达式: ";
    cin >> expression;
    // 判断括号是否配对
    if (isParenthesesMatched(expression)) {
        cout << "括号配对正确！" << endl;
    } else {
        cout << "括号配对错误！" << endl;
    }

    return 0;
}
