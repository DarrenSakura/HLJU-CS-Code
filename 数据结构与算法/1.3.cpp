#include <iostream>
using namespace std;

const int MAXSIZE = 100;

struct SeqList {
	int data[MAXSIZE];
	int length;
};

// 仅用一个辅助结点（temp）将顺序表逆置
void reverseList(SeqList &S) {
	int i = 0, j = S.length - 1;
	while (i < j) {
		int temp = S.data[i];
		S.data[i] = S.data[j];
		S.data[j] = temp;
		++i;
		--j;
	}
}

void printList(const SeqList &S) {
	for (int i = 0; i < S.length; ++i) {
		cout << S.data[i] << (i + 1 == S.length ? '\n' : ' ');
	}
}

int main() {
	SeqList S = {{1, 2, 3, 4, 5, 6}, 6};

	reverseList(S);
	printList(S); // 期望输出: 6 5 4 3 2 1

	return 0;
}
