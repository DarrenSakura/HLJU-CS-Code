#include<iostream>
using namespace std;
#define OK 1
#define Maxsize 100
#define ERROR 0
#define OVERFLOW -2
typedef int Status;
typedef struct LNode
{
    int data;
    LNode *next;
}LNode,*LinkList;
Status CreateLink1(LinkList &L)
{
    LinkList p;
    L=new LNode;
    //if(L=NULL)
      //  return ERROR;
    L->next=NULL;
    for(int i=0;i<5;i++)
    {
        p=new LNode;
        cin>>p->data;
        p->next=L->next;
        L->next=p;
    }
    return OK;
}//前插法
Status CreateLink2(LinkList &L)
{
    LinkList p;
    L=new LNode;
    L->next=NULL;
    LinkList r=L;
    cout<<"输入5个数：";
    for(int i=0;i<5;i++)
    {
        p=new LNode;
        cin>>p->data;
        p->next=NULL;
        r->next=p;
        r=p;
    }
    return OK;
}//尾插法
Status Insert(LinkList &L)
{
    cout<<"输入要插入的数：";
    LinkList p,q;
    p=new LNode;
    q=L;
    cin>>p->data;
    if(p->data<q->next->data)
    {
        p->next=q->next;
        q->next=p;
        return OK;
    }
    LinkList tail;
    tail=L;
    while(tail->next)
    {
        tail=tail->next;
    }
    if(p->data>tail->data)
    {
        tail->next=p;
        p->next=NULL;
    }
    q=L;
    if(p->data>=q->next->data&&p->data<=tail->data)
    {
        while(q->next->data<p->data)
        {
            q=q->next;
        }
        p->next=q->next;
        q->next=p;
    }
    return OK;
}
Status Printf(LinkList &L)
{
    LinkList p;
    p=L->next;
    while(p)
    {
        cout<<p->data<<"->";
        p=p->next;
    }
    return OK;
}
int main()
{
    LinkList L,p;
    CreateLink2(L);
    Insert(L);
    Printf(L);
}
