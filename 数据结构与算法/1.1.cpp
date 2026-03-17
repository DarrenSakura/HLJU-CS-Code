#include <iostream>
using namespace std;

const int MAXSIZE = 100;

struct SeqList {
    int data[MAXSIZE];
    int length; // 当前元素个数
};

// 在递增有序顺序表中插入 x，成功返回 true，失败返回 false（表满）
bool insertOrdered(SeqList &S, int x) {
    if (S.length >= MAXSIZE) return false; // 表满

    int i = S.length - 1;

    // 从后向前找插入位置，同时后移元素
    while (i >= 0 && S.data[i] > x) {
        S.data[i + 1] = S.data[i];
        --i;
    }

    S.data[i + 1] = x; // 插入
    ++S.length;
    return true;
}

int main() {
    SeqList S = {{1, 3, 5, 7, 9}, 5};
    int x = 6;

    if (insertOrdered(S, x)) {
        for (int i = 0; i < S.length; ++i) {
            cout << S.data[i] << " ";
        }
        cout << endl; // 输出: 1 3 5 6 7 9
    } else {
        cout << "插入失败：顺序表已满" << endl;
    }

    return 0;
}