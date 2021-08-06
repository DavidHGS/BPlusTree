#include "BPlus_Node.h"
#include <queue>
using namespace std;
/******************节点***************/
Node::Node(/* args */)
{
    m_nkeynum=0;
    m_bLeaf=false;
}

Node::~Node()
{
}

/******************内部节点***************/
InternalNode::InternalNode(/* args */)
{
    internalNodeData=new struct InternalNodeData;
    for(int &m_key : internalNodeData->m_keys){
        m_key=0;//初始化内部节点关键字
    }
    for(int i=0;i<LEAF_ORDER;i++){
        internalNodeData->m_pointers[i]=nullptr;
    }
}

InternalNode::~InternalNode()
{
}
/******************叶子节点***************/
LeafNode::LeafNode(/* args */)
{
    leafNodeData=new struct LeafNodeData;
    for(int i=0;i<ORDER;i++){
        leafNodeData->m_keys[i]=0;
    }
    for(int i=0;i<ORDER;i++){
        leafNodeData->m_number[i]=0.0;
    }
    leafNodeData->next=nullptr;
}

LeafNode::~LeafNode()
{
}
/******************BplusTree***************/
BPlusTree::BPlusTree()
{
    //树的初始化
    m_pRoot = nullptr;
    m_pFirst = nullptr;
    m_pLast = nullptr;
}

BPlusTree::~BPlusTree() = default;

bool BPlusTree::Insert(int nkey, double number)
{
    int i = 0;
    int m = 0;

    //根节点为空，进行初始化
    if (m_pRoot == nullptr)
    {
        auto *pNew = new LeafNode();
        //申请空间失败返回false
        pNew->m_bLeaf = true;
        pNew->m_nkeynum = 1;
        pNew->m_pParent = nullptr;
        pNew->leafNodeData->m_keys[0] = nkey;
        pNew->leafNodeData->m_number[0] = number;
        m_pRoot = pNew;
        m_pFirst = pNew;
        return true;
    }

    //找到插入的叶子节点
    Node *pTmp = m_pRoot;

    //当前节点不是叶子节点,向下寻找
    while (!pTmp->m_bLeaf)
    {
        for (int i = 0; i < pTmp->m_nkeynum; i++)
        {
            if (nkey < ((InternalNode *)pTmp)->internalNodeData->m_keys[i])
                break;
        }
        if (i == 0)
            pTmp = (Node *)(((InternalNode *)pTmp)->internalNodeData->m_pointers[0]);
        else
        {
            pTmp = (Node *)(((InternalNode *)pTmp)->internalNodeData->m_pointers[1]);
        }
    }

    //判断叶节点是否存在
    for (i = 0; i < ((LeafNode *)pTmp)->m_nkeynum; i++)
    {
        //想要插入的叶子节点已经存在
        if (nkey == ((LeafNode *)pTmp)->leafNodeData->m_number[i])
        {
            return false;
        }
    }
    //判断叶子节点的数量(判断能否插入)
    if (((LeafNode *)pTmp)->m_nkeynum < LEAF_ORDER)
    {
        for (i = 0; i < ((LeafNode *)pTmp)->m_nkeynum; i++)
        {
            if (nkey < ((LeafNode *)pTmp)->leafNodeData->m_number[i])
            {
                break; //找到叶子节点中第一个比关键字小的即为插入点(找不到则跳出if)
            }
        }
        //叶子节点关键字移动
        for (m = ((LeafNode *)pTmp)->m_nkeynum - 1; m >= i; m--)
        {
            ((LeafNode *)pTmp)->leafNodeData->m_keys[m + 1] = ((LeafNode *)pTmp)->leafNodeData->m_keys[m];
            ((LeafNode *)pTmp)->leafNodeData->m_number[m + 1] = ((LeafNode *)pTmp)->leafNodeData->m_number[m];
        }
        //插入
        ((LeafNode *)pTmp)->leafNodeData->m_keys[i] = nkey;
        ((LeafNode *)pTmp)->leafNodeData->m_number[i] = number;
        double x1 = ((LeafNode *)pTmp)->leafNodeData->m_number[i];
        ((LeafNode *)pTmp)->m_nkeynum++; //叶子节点关键字增加
        return true;                     //插入成功
    }

    //叶子节点已满
    int nMid = (LEAF_ORDER + 1) / 2;
    //临时存储待插入节点信息
    int *pTmpKeys = new int[LEAF_ORDER + 1];
    auto *pTmpnumbers = new double[LEAF_ORDER + 1];

    for (i = 0; i < pTmp->m_nkeynum; i++)
    {
        if (((LeafNode *)pTmp)->leafNodeData->m_keys[i] > nkey)
        {
            break; //if i指向第一个比nkey大的节点
        }
        //else i为pTmp->m_nkeynum-1
    }

    //临时叶子节点加入新插入的值
    for (m = pTmp->m_nkeynum - 1; m >= i; i--)
    {
        pTmpKeys[m + 1] = ((LeafNode *)pTmp)->leafNodeData->m_keys[m];
        pTmpnumbers[m + 1] = ((LeafNode *)pTmp)->leafNodeData->m_number[m];
    }

    for (m = 0; m < i; m++)
    {
        pTmpKeys[m] = ((LeafNode *)pTmp)->leafNodeData->m_keys[m];
        pTmpnumbers[m] = ((LeafNode *)pTmp)->leafNodeData->m_number[m];
        double x = ((LeafNode *)pTmp)->leafNodeData->m_number[m];
        double y = pTmpnumbers[m]; //m=i-1
    }
    pTmpKeys[i] = nkey;
    pTmpnumbers[i] = number;

    //新建叶子节点用于分裂(复制pTmp)
    auto *pNew = new LeafNode;

    for (m = 0, i = nMid; i < LEAF_ORDER + 1; i++, m++)
    {
        pNew->leafNodeData->m_keys[m] = pTmpKeys[i];
        pNew->leafNodeData->m_number[m] = pTmpnumbers[i];
        pNew->m_nkeynum++;
    }

    pNew->m_pParent = pTmp->m_pParent;

    pNew->m_bLeaf = pTmp->m_bLeaf;

    //将pTmp节点清空
    for (i = 0; i < pTmp->m_nkeynum; i++)
    {
        ((LeafNode *)pTmp)->leafNodeData->m_keys[i] = 0;
        ((LeafNode *)pTmp)->leafNodeData->m_number[1] = 0.0;
    }
    pTmp->m_nkeynum = 0;

    for (i = 0; i < nMid; i++)
    {
        ((LeafNode *)pTmp)->leafNodeData->m_keys[i] = pTmpKeys[i];
        ((LeafNode *)pTmp)->leafNodeData->m_number[i] = pTmpnumbers[i];
        pTmp->m_nkeynum++;
    }
    pNew->leafNodeData->next = ((LeafNode *)pTmp)->leafNodeData->next;
    ((LeafNode *)pTmp)->leafNodeData->next = pNew; //防止Tmp节点左侧断链

    if (!InsertKeyAndPointer(pTmp->m_pParent, pTmp, pTmpKeys[nMid], pNew))
    {
        return false;
    }
    return true;
}

void BPlusTree::PrintLeaves()
{
    int i = 0;
    if (!m_pFirst)
        return;
    LeafNode *pCur = (LeafNode *)m_pFirst;
    printf("*****************print leaves****************\n");
    while (1)
    {
        for (int i = 0; i < pCur->m_nkeynum; i++)
        {
            printf("<key:%d,number%lf\n", pCur->leafNodeData->m_keys[i], pCur->leafNodeData->m_number[i]);
        }
        if (pCur->leafNodeData->next == NULL)
        {
            break;
        }
        else
        {
            pCur = static_cast<LeafNode *>(pCur->leafNodeData->next);
        }
        printf("******************************************\n");
    }
}

bool BPlusTree::InsertKeyAndPointer(Node *pParent, Node *pOld, int nkey, Node *pNew) //已分裂节点 左节点 关键字 右节点
{
    if (!pOld)
        return false;
    if (!pNew)
        return false;
    int i = 0;
    int m = 0;
    int k = 0;

    //父节点不存在，就是根节点的父亲
    if (pParent == nullptr)
    {
        //产生新根节点
        auto *pNewRoot = new InternalNode;
        //申请失败则为空
        pNewRoot->m_bLeaf = false;
        pNewRoot->internalNodeData->m_keys[0] = nkey;
        pNewRoot->m_nkeynum = 1;
        pNewRoot->internalNodeData->m_pointers[0] = pOld;
        pNewRoot->internalNodeData->m_pointers[1] = pNew;
        pNewRoot->m_pParent = nullptr;
        pOld->m_pParent = pNewRoot;
        pNew->m_pParent = pNewRoot;
        m_pRoot = pNewRoot;
        return true;
    }

    //挪一下
    for (i = 0; i < pParent->m_nkeynum; i++)
    {
        //不可以添加重复的值
        if (nkey == ((InternalNode *)pParent)->internalNodeData->m_keys[i])
            ;
        return false;
        if (nkey < ((InternalNode *)pParent)->internalNodeData->m_keys[i])
            ;
        break;
    }

    //判断父节点是否已经满了
    if (pParent->m_nkeynum < ORDER)
    {
        //移动大于nkey的关键字
        for (m = pParent->m_nkeynum - 1; m >= i; m--)
        {
            ((InternalNode *)pParent)->internalNodeData->m_keys[m + 1] = ((InternalNode *)pParent)->internalNodeData->m_keys[m];
        }
        //移动孩子指针
        for (m = pParent->m_nkeynum; m > i; m--)
        {
            ((InternalNode *)pParent)->internalNodeData->m_pointers[m + 1] = ((InternalNode *)pParent)->internalNodeData->m_pointers[m];
        }

        //插进去
        ((InternalNode *)pParent)->internalNodeData->m_keys[i] = nkey;
        ((InternalNode *)pParent)->internalNodeData->m_pointers[i + 1] = pNew;
        pParent->m_nkeynum++;

        pNew->m_pParent = pParent;
        return true;
    }
    //父节点已满
    int *pTmpKeys = new int[ORDER + 1];

    void **pTmpPointers = new void *[ORDER + 2];

    //找到插入的节点
    for (i = 0; i < pParent->m_nkeynum; i++)
    {
        if (nkey < ((InternalNode *)pParent)->internalNodeData->m_keys[i])
        {
            break;
        }
    }

    //键插入
    for (m = 0; m < i; m++)
    {
        pTmpKeys[m] = ((InternalNode *)pParent)->internalNodeData->m_keys[i];
    }

    pTmpKeys[m] = nkey;
    m++;

    for (k = i; k < pParent->m_nkeynum; k++, m++)
    {
        pTmpKeys[m] = ((InternalNode *)pParent)->internalNodeData->m_keys[k];
    }

    //孩子指针插入
    for (m = 0; m <= i; m++)
    {
        pTmpPointers[m] = ((InternalNode *)pParent)->internalNodeData->m_pointers[m];
    }
    pTmpPointers[m] = pNew;
    m++;
    for (k = i + 1; k <= pParent->m_nkeynum; k++, m++)
    {
        pTmpPointers[m] = ((InternalNode *)pParent)->internalNodeData->m_pointers[k];
    }

    //父节点分类，分裂出的节点初始化
    Node *pNewSplit = new InternalNode;

    //设置是不是叶子节点
    pNewSplit->m_bLeaf = pParent->m_bLeaf;

    //新分裂出的节点
    pNewSplit->m_pParent = pParent->m_pParent;

    //从这儿分开
    int nMid = (ORDER + 1) / 2;

    //提到父节点
    int nMidKey = pTmpKeys[nMid];

    //右边的节点
    for (m = 0, i = nMid + 1; i < ORDER + 1; i++, m++)
    {
        ((InternalNode *)pNewSplit)->internalNodeData->m_keys[m] = pTmpKeys[i];
        ((InternalNode *)pNewSplit)->internalNodeData->m_pointers[m] = pTmpPointers[i];

        if (!pNewSplit->m_bLeaf)
        { //不是叶子节点
            Node *pCur = static_cast<Node *>(((InternalNode *)pNewSplit)->internalNodeData->m_pointers[m]);
            if (pCur)
            {
                pCur->m_pParent = pNewSplit; //将存在的子节点指向新分裂的节点
            }
        }
        pNewSplit->m_nkeynum++;
    }

    ((InternalNode *)pNewSplit)->internalNodeData->m_pointers[m] = pTmpPointers[i];

    if (!pNewSplit->m_bLeaf)
    {
        Node *pCur = static_cast<Node *>(((InternalNode *)pNewSplit)->internalNodeData->m_pointers[m]);
        if (pCur)
        {
            pCur->m_pParent = pNewSplit;
        }
        pParent->m_nkeynum = 0;
    }
    //左边节点
    for (i = 0; i < nMid; i++)
    {
        ((InternalNode *)pParent)->internalNodeData->m_keys[i] = pTmpKeys[i];
        ((InternalNode *)pParent)->m_nkeynum++;
    }

    for (i = 0; i < nMid; i++)
    {
        ((InternalNode *)pParent)->internalNodeData->m_pointers[i] = pTmpPointers[i];
    }

    //释放临时节点
    delete[] pTmpPointers;
    delete[] pTmpKeys;
    return InsertKeyAndPointer(pParent->m_pParent, pParent, nMidKey, pNewSplit);
}

//查找某个值是否存在
bool BPlusTree::search(int nkey)
{
    //向下直到找到叶子节点

    int i = 0;
    //树上没有节点

    if (!m_pRoot)
        return false;
    Node *pTmp = m_pRoot;
    while (!pTmp->m_bLeaf)
    {
        for (i = 0; i < pTmp->m_nkeynum; i++)
        {
            if (nkey < ((InternalNode *)pTmp)->internalNodeData->m_keys[i])
            {
                break; //可修改为二分查找
            }
        }
        pTmp = (Node *)(((InternalNode *)pTmp)->internalNodeData->m_pointers[i]);
    }

    auto *leafNode = (LeafNode *)pTmp;
    for (i = 0; i < leafNode->m_nkeynum; i++)
    {
        if (nkey == leafNode->leafNodeData->m_keys[i])
        {
            return true;
        }
    }
    return false;
}

void BPlusTree::PrintLayerTree()
{
    int i = 0;
    queue<Node *> q;
    if (m_pRoot == NULL)
    {
        printf("b+tree is null!\n");
    }

    Node *node;

    node = m_pRoot;
    q.push(node);
    while (q.empty() == false)
    {
        Node *nodeTmp = q.front();

        if (nodeTmp->m_bLeaf == false) //不是叶子节点
        {
            for (i = 0; i < nodeTmp->m_nkeynum; i++)
            {
                printf("%d", ((InternalNode *)nodeTmp)->internalNodeData->m_keys[i]);
                q.push((Node *)((InternalNode *)nodeTmp)->internalNodeData->m_pointers[i]);
                printf(" ");
            }
            q.push((Node *)((InternalNode *)nodeTmp)->internalNodeData->m_pointers[i]);
        }
        else
        {
            for (i = 0; i < nodeTmp->m_nkeynum; i++)
            {
                printf("%d", ((LeafNode *)nodeTmp)->leafNodeData->m_keys[i]);
                printf(" ");
            }
        }
        printf("\n");
        printf("**********************************************\n");
        q.pop();
    }
    return;
}

bool BPlusTree::Remove(int nkey)
{
    //树上没有节点
    if (!m_pRoot)
        return false;

    int i = 0;
    int j = -1;
    int m = 0;

    Node *pTmp = m_pRoot;

    //向下查找，直到找到叶子节点
    while (!pTmp->m_bLeaf)
    {
        bool flag = false;
        for (i = 0; i < pTmp->m_nkeynum; i++)
        {
            if (nkey < ((InternalNode *)pTmp)->internalNodeData->m_keys[i])
            {
                flag = true; //查找到删除节点
                j = i;
                break;
            }
        }
        if (flag == false)
        {
            j = pTmp->m_nkeynum;
        }

        pTmp = (Node *)(((InternalNode *)pTmp)->internalNodeData->m_pointers[i]);
    }

    //找到应该被删除的节点
    for (i = 0; i < pTmp->m_nkeynum; i++)
    {
        if (nkey < ((LeafNode *)pTmp)->leafNodeData->m_keys[i])
        {
            break;
        }
    }
    //没有找到被删除的节点
    if (i == pTmp->m_nkeynum)
    {
        printf("%d node not exists!\n", nkey);
        return false;
    }

    auto *pCur = (LeafNode *)pTmp;

    //剩下的节点向前移动，挤掉被删除的节点
    for (m = i + 1; m < pTmp->m_nkeynum; m++)
    {
        pCur->leafNodeData->m_keys[m - 1] = pCur->leafNodeData->m_keys[m];
        pCur->leafNodeData->m_number[m - 1] = pCur->leafNodeData->m_number[m];
    }
    pTmp->m_nkeynum--;

    //当节点的数目大于(m+1)/2的值时，直接删除
    int nLowNum = (LEAF_ORDER + 1) / 2;
    if (pTmp->m_nkeynum >= nLowNum)
    {
        return true;
    }

    //当节点的数目小于小面的值时，就应该进行之后的操作了
    //下溢出
    Node *pParent = pTmp->m_pParent;

    //父亲节点是空，说明当前节点是根节点
    if (!pParent)
    {
        //根节点，删空了
        if (pTmp->m_nkeynum < 1)
        {
            m_pRoot = NULL;
            delete pTmp;
            m_pFirst = m_pLast = NULL;
        }
        return true;
    }

    Node *pNeighbor = NULL;
    int nNeighbor = -1;
    int nIndex = -1;

    //当前节点位置为头节点，邻居节点为1号节点
    if (j == 0)
    {
        pNeighbor = (Node *)(((InternalNode *)pParent)->internalNodeData->m_pointers[1]);
        nNeighbor = j - 1;
        nIndex = j;
    }
    else
    {
        pNeighbor = (Node *)(((InternalNode *)pParent)->internalNodeData->m_pointers[j - 1]);
        nNeighbor = j - 1;
        nIndex = j;
    }

    //若兄弟节点key有富余(大于Math.ceil(m-1)/2-1),向兄弟节点借一个记录，同时用借到的key替换父节点
    if (pNeighbor->m_nkeynum > nLowNum)
    {
        //向左邻居借一个节点
        if (nNeighbor < nIndex)
        {
            //左邻居借出最右边节点，上升到父亲节点
            ((InternalNode *)pParent)->internalNodeData->m_keys[nNeighbor] = ((LeafNode *)pNeighbor)->leafNodeData->m_keys[pNeighbor->m_nkeynum - 1];
            //该节点为借过来的节点挪地方
            for (i = pTmp->m_nkeynum - 1; i >= 0; i--)
            {
                ((LeafNode *)pNeighbor)->leafNodeData->m_keys[i + 1] = ((LeafNode *)pNeighbor)->leafNodeData->m_keys[i];
                ((LeafNode *)pNeighbor)->leafNodeData->m_number[i + 1] = ((LeafNode *)pNeighbor)->leafNodeData->m_number[i];
            }
            ((LeafNode *)pNeighbor)->leafNodeData->m_keys[i + 1] = ((LeafNode *)pNeighbor)->leafNodeData->m_keys[pNeighbor->m_nkeynum + 1];
            ((LeafNode *)pNeighbor)->leafNodeData->m_number[0] = ((LeafNode *)pNeighbor)->leafNodeData->m_number[pNeighbor->m_nkeynum + 1];
            pTmp->m_nkeynum++;
            pNeighbor->m_nkeynum--;
        }
        //向右邻居借出一个节点
        else
        {
            ((InternalNode *)pParent)->internalNodeData->m_keys[nIndex] = ((LeafNode *)pNeighbor)->leafNodeData->m_keys[1];
            ((LeafNode *)pTmp)->leafNodeData->m_keys[pTmp->m_nkeynum] = ((LeafNode *)pTmp)->leafNodeData->m_keys[0];
            ((LeafNode *)pTmp)->leafNodeData->m_number[pTmp->m_nkeynum] = ((LeafNode *)pTmp)->leafNodeData->m_number[0];
            pTmp->m_nkeynum++;
            for (i = 1; i <= pNeighbor->m_nkeynum - 1; i++)
            {
                ((LeafNode *)pNeighbor)->leafNodeData->m_keys[i - 1] = ((LeafNode *)pNeighbor)->leafNodeData->m_keys[i];
                ((LeafNode *)pNeighbor)->leafNodeData->m_number[i - 1] = ((LeafNode *)pNeighbor)->leafNodeData->m_number[i];
            }
            pNeighbor->m_nkeynum--;
        }
        return true;
    }

    //3)若兄弟节点中没有富余的key，则当前节点和兄弟节点合并成一个新的叶子节点，并且删除父节点中的key
    //（父结点中的这个key两边的孩子指针就变成了一个指针，正好指向这个新的叶子结点），
    // 将当前结点指向父结点（必为索引结点）。
    else
    {
        //合并左邻居和自己
        if (nNeighbor < nIndex)
        {
            for (i = 0; i < pTmp->m_nkeynum; i++)
            {
                ((LeafNode *)pNeighbor)->leafNodeData->m_keys[pNeighbor->m_nkeynum] = ((LeafNode *)pTmp)->leafNodeData->m_keys[i];
                ((LeafNode *)pNeighbor)->leafNodeData->m_number[pNeighbor->m_nkeynum] = ((LeafNode *)pTmp)->leafNodeData->m_number[i];
                pNeighbor->m_nkeynum++;
            }
            //删除父节点中的一个键
            for (i = nIndex; i < pParent->m_nkeynum; i++)
            {
                ((InternalNode *)pParent)->internalNodeData->m_keys[i - 1] = ((InternalNode *)pParent)->internalNodeData->m_keys[i];
            }
            for (i = nIndex + 1; i <= pParent->m_nkeynum; i++)
            {
                ((InternalNode *)pParent)->internalNodeData->m_pointers[i - 1] = ((InternalNode *)pParent)->internalNodeData->m_pointers[i];
            }

            pParent->m_nkeynum--;
            ((LeafNode *)pNeighbor)->leafNodeData->next = ((LeafNode *)pTmp)->leafNodeData->next;
            delete pTmp;
        }
        else
        { //合并右邻居和自己
            for (i = 0; i < pNeighbor->m_nkeynum; i++)
            {
                ((LeafNode *)pTmp)->leafNodeData->m_keys[pTmp->m_nkeynum] = ((LeafNode *)pNeighbor)->leafNodeData->m_keys[i];
                ((LeafNode *)pTmp)->leafNodeData->m_number[pTmp->m_nkeynum] = ((LeafNode *)pNeighbor)->leafNodeData->m_number[i];
                pTmp->m_nkeynum++;
            }
            for (i = nNeighbor; i < pParent->m_nkeynum; i++)
            {
                ((InternalNode *)pParent)->internalNodeData->m_keys[i - 1] = ((InternalNode *)pParent)->internalNodeData->m_keys[i];
            }
            for (i = nNeighbor; i <= pNeighbor->m_nkeynum; i++)
            {
                ((InternalNode *)pParent)->internalNodeData->m_pointers[i - 1] = ((InternalNode *)pParent)->internalNodeData->m_pointers[i];
            }
            pParent->m_nkeynum--;
            ((LeafNode *)pTmp)->leafNodeData->next = ((LeafNode *)pNeighbor)->leafNodeData->next;
            delete pNeighbor;
        }

        //当前节点指向父节点
        Node *pCurTmp = pParent;
        int nInternalLowNum = (ORDER + 1) / 2;

        //分支节点重复操作
        while (pCurTmp)
        {
            //若索引节点的key的个数大于等于Math.ceil(m-1)/2-1,则删除操作结束
            if (pCurTmp->m_nkeynum >= nInternalLowNum)
            {
                break;
            }
            //借或者合并
            Node *pCurParent = pCurTmp->m_pParent;
            Node *pCurNeighbor = NULL;

            int nCurIndex = 0;
            int nNeighborIndex = 0;
            int nTmp = 0;
            //当前节点为根节点
            if (!pCurParent)
            {
                //根节点
                if (pCurTmp->m_nkeynum < 1)
                {
                    //删除当前根节点，更新根节点为第一个孩子
                    ((Node *)(((InternalNode *)pTmp)->internalNodeData->m_pointers[0]))->m_pParent = NULL;
                    m_pRoot = (Node *)(((InternalNode *)pCurTmp)->internalNodeData->m_pointers[0]);
                    delete pCurTmp;
                }
                break;
            }
            else
            {
                //非根节点
                //找到当前节点位置
                for (i = 0; i <= pCurParent->m_nkeynum; i++)
                {
                    if (pCurTmp == ((InternalNode *)pParent)->internalNodeData->m_pointers[i])
                        ;
                    nCurIndex = 0;
                    nNeighborIndex = 1;
                }
                if (i == 0)
                {
                    pCurNeighbor = (Node *)((InternalNode *)pCurParent)->internalNodeData->m_pointers[1];
                    nCurIndex = 0;
                    nNeighborIndex = i - 1;
                }
                else
                {
                    pCurNeighbor = (Node *)((InternalNode *)pCurParent)->internalNodeData->m_pointers[i - 1];
                    nCurIndex = i;
                    nNeighborIndex = i - 1;
                }
                //兄弟节点有能力借
                if (pCurNeighbor->m_nkeynum > nInternalLowNum)
                {
                    //借
                    if (nNeighborIndex < nCurIndex)
                    {
                        //左借
                        nTmp = ((InternalNode *)pCurParent)->internalNodeData->m_keys[nNeighborIndex];
                        //父亲节点键值更改
                        ((InternalNode *)pCurParent)->internalNodeData->m_keys[nNeighborIndex] = ((InternalNode *)pCurNeighbor)->internalNodeData->m_keys[pCurNeighbor->m_nkeynum - 1];
                        for (i = pCurTmp->m_nkeynum - 1; i >= 0; i--)
                        {
                            ((InternalNode *)pCurTmp)->internalNodeData->m_keys[i + 1] = ((InternalNode *)pCurNeighbor)->internalNodeData->m_keys[pCurNeighbor->m_nkeynum - 1];
                        }
                        for (i = pCurTmp->m_nkeynum; i >= 0; i--)
                        {
                            ((InternalNode *)pCurTmp)->internalNodeData->m_pointers[i + 1] = ((InternalNode *)pCurTmp)->internalNodeData->m_pointers[i];
                        }
                        ((InternalNode *)pCurTmp)->internalNodeData->m_keys[0] = nTmp;
                        if (((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[pCurNeighbor->m_nkeynum])
                        {
                            static_cast<Node *>(((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[pCurNeighbor->m_nkeynum])->m_pParent = pCurTmp;
                            ((InternalNode *)pCurTmp)->internalNodeData->m_pointers[0] = ((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[pCurNeighbor->m_nkeynum];
                            pCurTmp->m_nkeynum++;
                            pCurNeighbor->m_nkeynum--;
                        }
                        else
                        {
                            //右借
                            nTmp = ((InternalNode *)pCurParent)->internalNodeData->m_keys[nCurIndex];
                            ((InternalNode *)pCurParent)->internalNodeData->m_keys[nCurIndex] = ((InternalNode *)pCurNeighbor)->internalNodeData->m_keys[0];
                            ((InternalNode *)pCurTmp)->internalNodeData->m_keys[pCurTmp->m_nkeynum] = nTmp;

                            //右面节点不是空
                            if (((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[0])
                            {
                                static_cast<Node *>(((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[0])->m_pParent = pCurTmp;
                            }

                            ((InternalNode *)pCurTmp)->internalNodeData->m_pointers[pCurTmp->m_nkeynum + 1] = ((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[0];
                            pCurTmp->m_nkeynum++;

                            for (i = 1; i < pCurNeighbor->m_nkeynum; i++)
                            {
                                ((InternalNode *)pCurNeighbor)->internalNodeData->m_keys[i - 1] = ((InternalNode *)pCurNeighbor)->internalNodeData->m_keys[i];
                            }

                            for (i = 1; i <= pCurNeighbor->m_nkeynum; i++)
                            {
                                ((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[i - 1] = ((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[i];
                            }
                            pCurNeighbor->m_nkeynum--;
                        }
                        break;
                    }
                    //兄弟节点没有能力借，当前节点和兄弟节点及父节点下移合成应该新的节点。将当前节点指向父节点，重复第四步
                    else
                    {
                        //左借借不到
                        if (nNeighborIndex < nCurIndex)
                        {
                            //左合并
                            //合并出一个新的节点
                            ((InternalNode *)pCurNeighbor)->internalNodeData->m_keys[pCurNeighbor->m_nkeynum] = ((InternalNode *)pCurParent)->internalNodeData->m_keys[nNeighborIndex];
                            pCurNeighbor->m_nkeynum++;
                            for (i = 0; i < pCurTmp->m_nkeynum; i++)
                            {
                                ((InternalNode *)pCurNeighbor)->internalNodeData->m_keys[pCurNeighbor->m_nkeynum] = ((InternalNode *)pCurTmp)->internalNodeData->m_keys[i];
                                ((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[pCurNeighbor->m_nkeynum] = ((InternalNode *)pCurTmp)->internalNodeData->m_pointers[i];
                                Node *pChild = (Node *)((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[pCurNeighbor->m_nkeynum];
                                if (pChild)
                                    pChild->m_pParent = pCurNeighbor;
                                pCurNeighbor->m_nkeynum++;
                            }
                            ((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[pCurNeighbor->m_nkeynum] = ((InternalNode *)pCurTmp)->internalNodeData->m_pointers[i];

                            if (((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[pCurNeighbor->m_nkeynum])
                            {
                                static_cast<Node *>(((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[pCurNeighbor->m_nkeynum])->m_pParent = pCurNeighbor;
                            }

                            //父结点调整
                            for (i = nNeighborIndex + 1; i < pCurParent->m_nkeynum; i++)
                            {
                                ((InternalNode *)pCurParent)->internalNodeData->m_keys[i - 1] = ((InternalNode *)pCurParent)->internalNodeData->m_keys[i];
                            }

                            for (i = nCurIndex + 1; i <= pCurParent->m_nkeynum; i++)
                            {
                                ((InternalNode *)pCurParent)->internalNodeData->m_pointers[i - 1] = ((InternalNode *)pCurParent)->internalNodeData->m_pointers[i];
                            }

                            pCurParent->m_nkeynum--;

                            delete pCurTmp;
                        }
                        else
                        { //右借借不到
                            //右合并
                            //形成合并节点
                            ((InternalNode *)pCurTmp)->internalNodeData->m_keys[pCurTmp->m_nkeynum] = ((InternalNode *)pCurParent)->internalNodeData->m_keys[nCurIndex];
                            pCurTmp->m_nkeynum++;

                            for (i = 0; i < pCurNeighbor->m_nkeynum; i++)
                            {
                                ((InternalNode *)pCurTmp)->internalNodeData->m_keys[pCurTmp->m_nkeynum] = ((InternalNode *)pCurNeighbor)->internalNodeData->m_keys[i];
                                ((InternalNode *)pCurTmp)->internalNodeData->m_pointers[pCurTmp->m_nkeynum] = ((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[i];
                                Node *pChild = (Node *)(((InternalNode *)pCurTmp)->internalNodeData->m_pointers[pCurTmp->m_nkeynum]);
                                if (pChild)
                                    pChild->m_pParent = pCurTmp;
                                pCurTmp->m_nkeynum++;
                            }

                            ((InternalNode *)pCurTmp)->internalNodeData->m_pointers[pCurTmp->m_nkeynum] = ((InternalNode *)pCurNeighbor)->internalNodeData->m_pointers[i];
                            if (((InternalNode *)pCurTmp)->internalNodeData->m_pointers[pCurTmp->m_nkeynum])
                            {
                                ((Node *)(((InternalNode *)pCurTmp)->internalNodeData->m_pointers[pCurTmp->m_nkeynum]))->m_pParent = pCurTmp;
                            }

                            //父结点调整
                            for (i = nCurIndex + 1; i < pCurParent->m_nkeynum; i++)
                            {
                                ((InternalNode *)pCurParent)->internalNodeData->m_keys[i - 1] = ((InternalNode *)pCurParent)->internalNodeData->m_keys[i];
                            }

                            for (i = nNeighborIndex + 1; i <= pCurParent->m_nkeynum; i++)
                            {
                                ((InternalNode *)pCurParent)->internalNodeData->m_pointers[i - 1] = ((InternalNode *)pCurParent)->internalNodeData->m_pointers[i];
                            }

                            pCurParent->m_nkeynum--;

                            delete pCurNeighbor;
                        }
                        pCurTmp = pCurParent;
                    }
                }
            }
            return true;
        }
    }
}