#include <stdio.h>
//非递归实现完全二叉树的前序遍历
void preOrderTraversal(int tree[], int n)
{
    if (n == 0) return; //如果树为空则返回
    int stack[n];  //定义一个栈
    int top = -1;  //初始化栈顶指针
    int index = 0; //当前节点下标
    stack[++top] = 0;//将根节点入栈
    while (top != -1)
    {
        index = stack[top--];//弹出栈顶元素，index设为栈顶值，将top减1
        printf("%d ", tree[index]);//打印当前节点的值
        if (2 * index + 2 < n)//有没有右子树
            stack[++top] = 2 * index + 2;//右子节点压栈
        if (2 * index + 1 < n)//有没有左子树
            stack[++top] = 2 * index + 1;//左子节点压栈
    }
}
int main()
{
    int tree[] = {1,2,3,4,5,6,7};
    int n = sizeof(tree)/sizeof(tree[0]);
    preOrderTraversal(tree,n);
    return 0;
}
