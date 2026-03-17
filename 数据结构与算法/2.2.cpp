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
Status CreateLink(LinkList &L)
{
    LinkList p;
    L=new LNode;
    L->next=NULL;
    LinkList r=L;
    cout<<"输入5个数";
    for(int i=0;i<5;i++)
    {
        p=new LNode;
        cin>>p->data;
        p->next=NULL;
        r->next=p;
        r=p;
    }
    return OK;
}//Î²²å·¨
Status Reversal(LinkList &L)
{
    LinkList head,p,q,tail;
    head=L;
    tail=L;
    int count=0;
    p=head->next;
    q=p->next;
    while(tail->next)
    {
        tail=tail->next;
        count++;
    }
    for(int i=0;i<count-1;i++)
    {
        p->next=q->next;
        q->next=head->next;
        head->next=q;
        q=p->next;
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
    LinkList L;
    CreateLink(L);
    Reversal(L);
    Printf(L);
}
