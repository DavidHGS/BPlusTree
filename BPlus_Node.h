#pragma once
//只编译一次，会有版本兼容的风险

#define LEAF_ORDER 3
#define ORDER 4
#define MAX_NUM 5


#include<string>
#include<iostream>


/*
    第i颗子树的关键字都小于第i个关键字，
    关键字和子树的索引都从0开始的
*/



//40byte的非叶节点
struct InternalNodeData{
    //键值数组，指针数组
    int m_keys[ORDER];
    void *m_pointers[MAX_NUM];
};

struct LeafNodeData{
    int m_keys[LEAF_ORDER];
    double m_number[LEAF_ORDER];
    void* next;
};

/******************节点***************/
class Node
{
private:
    /* data */
public:
    
    int m_nkeynum;//键值的数量(Node)
    bool m_bLeaf;//是否为叶节点（Node）

    Node* m_pParent;//双亲节点指针（Node）
    Node(/* args */);
    ~Node();
};



/******************内部节点***************/
class InternalNode:public Node
{
private:
    /* data */
public:
    InternalNodeData *internalNodeData;
    InternalNode();
    ~InternalNode();
};




/******************叶子节点***************/
class LeafNode:public Node
{
private:
    /* data */
public:
    LeafNodeData *leafNodeData;
    LeafNode(/* args */);
    ~LeafNode();
};



/******************BplusTree***************/
class BPlusTree
{
private:
  
    //第一个叶子节点
    Node *m_pFirst;
    Node *m_pLast;
public:
    //根节点
    Node *m_pRoot;
    bool Insert(int nkey,double number);//叶子节点插入
    bool Remove(int nkey);//删除
    bool Update(int nkey,double number);//修改卫星节点数据
    bool search(int nkey);//查找某个值是否存在
    bool InsertKeyAndPointer(Node *pParent,Node *pOld,int nKey,Node *pNew);//内部节点插入
    void PrintLeaves();//打印叶子节点值
    
    void PrintLayerTree();//分层打印树
    BPlusTree(/* args */);
    ~BPlusTree();
};







