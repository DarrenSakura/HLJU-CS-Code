#include<stdio.h>
#include<stdlib.h>
struct TreeNode//定义二叉树节点
{
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
};
struct TreeNode* createNode(int data)//创建一个新节点
{
    struct TreeNode* newNode = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}
void preOrder(struct TreeNode* root)//非递归前序遍历
{
    if (root == NULL) return;//根为空
    struct TreeNode* stack[100];//定义栈
    int top = -1;//栈为空
    stack[++top] = root;//根入栈
    while (top != -1)//栈不为空
    {
        struct TreeNode* node = stack[top--];//弹出栈顶元素，将栈顶指针减1
        printf("%d ", node->data);//输出
        if (node->right != NULL)//有没有右子
            stack[++top] = node->right;//右子节点压栈
        if (node->left != NULL)//有没有左子
            stack[++top] = node->left;//左子节点压栈
    }
}
void inOrder(struct TreeNode* root)//非递归中序遍历
{
    struct TreeNode* stack[100];//定义栈
    int top = -1;//栈为空
    struct TreeNode* current = root;//curr指向root
    while (current != NULL || top != -1) //curr不为空或栈不为空
    {
        while (current != NULL)//curr不为空
        {
            stack[++top] = current;//curr入栈
            current = current->left;//curr指左子
        }
        current = stack[top--];//栈出一个节点给curr
        printf("%d ", current->data);//输出
        current = current->right;//curr指右子
    }
}
void postOrder(struct TreeNode* root)//非递归后序遍历
{
    if (root == NULL) return;//根为空
    struct TreeNode* stack1[100], *stack2[100];//定义两个栈
    int top1 = -1, top2 = -1;//两个栈为空
    stack1[++top1] = root;//将根节点压入栈1
    while (top1 != -1) //栈1不为空，进入循环
    {
        struct TreeNode* node = stack1[top1--];//栈1出一个节点
        stack2[++top2] = node;//进栈2
        if (node->left != NULL)//有左，入栈1
            stack1[++top1] = node->left;
        if (node->right != NULL)//有右，入栈1
            stack1[++top1] = node->right;
    }
    while (top2 != -1) //栈2不为空，进入循环
    {
        struct TreeNode* node = stack2[top2--];//栈2出一个节点
        printf("%d ", node->data);//输出
    }
}
int main()
{
    // 创建一个简单的二叉树
    struct TreeNode* root = createNode(1);
    root->left = createNode(2);
    root->right = createNode(3);
    root->left->left = createNode(4);
    root->left->right = createNode(5);
    root->right->left = createNode(6);
    root->right->right = createNode(7);
    // 前序遍历
    printf("Preorder traversal: ");
    preOrder(root);
    printf("\n");
    // 中序遍历
    printf("Inorder traversal: ");
    inOrder(root);
    printf("\n");
    // 后序遍历
    printf("Postorder traversal: ");
    postOrder(root);
    printf("\n");
    return 0;
}
