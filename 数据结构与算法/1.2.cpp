#include <iostream>
using namespace std;

const int MAXSIZE = 100;

struct SeqList {
	int data[MAXSIZE];
	int length;
};

// 逆置区间 [l, r]，仅用一个辅助变量 temp
void reverseRange(SeqList &S, int l, int r) {
	while (l < r) {
		int temp = S.data[l];
		S.data[l] = S.data[r];
		S.data[r] = temp;
		++l;
		--r;
	}
}

// 将顺序表循环右移 k 位
// 例如: 1 2 3 4 5 右移 2 位 -> 4 5 1 2 3
void rightShift(SeqList &S, int k) {
	int n = S.length;
	if (n <= 1) return;

	k %= n;
	if (k == 0) return;

	// 三次逆置法：
	// 1) 全部逆置
	// 2) 前 k 个逆置
	// 3) 后 n-k 个逆置
	reverseRange(S, 0, n - 1);
	reverseRange(S, 0, k - 1);
	reverseRange(S, k, n - 1);
}

void printList(const SeqList &S) {
	for (int i = 0; i < S.length; ++i) {
		cout << S.data[i] << (i + 1 == S.length ? '\n' : ' ');
	}
}

int main() {
	SeqList S = {{1, 2, 3, 4, 5, 6, 7}, 7};
	int k = 3;

	rightShift(S, k);
	printList(S); // 期望输出: 5 6 7 1 2 3 4

	return 0;
}