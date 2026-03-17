#include <stdio.h>
#include <stdlib.h>
// 定义二叉树节点
struct TreeNode
{
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
};
// 创建一个新节点
struct TreeNode* createNode(int data)
{
    struct TreeNode* newNode = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}
// 递归求二叉树的高度
int getHeight(struct TreeNode* root)
{
    if (root == NULL)//根空，树高度为0
        return 0;
    int leftHeight = getHeight(root->left);//算左
    int rightHeight = getHeight(root->right);//算右
    return (leftHeight >= rightHeight ? leftHeight : rightHeight) + 1;//取大加一（根）
}
int main()
{
    // 创建一个简单的二叉树
    struct TreeNode* root = createNode(1);
    root->left = createNode(2);
    root->right = createNode(3);
    root->left->left = createNode(4);
    root->left->right = createNode(5);
    // 输出二叉树的高度
    int height = getHeight(root);
    printf("树高度: %d\n", height);
    return 0;
}
