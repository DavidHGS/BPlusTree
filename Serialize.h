#include "BPlus_Node.h"
#include <queue>
using namespace std;

bool Serialize(Node *m_pRoot, char *path, long long *leafCount, long long *internalCount)
{
    *leafCount = *internalCount = 0;
    int i = 0;

    queue<Node *> q;

    if (m_pRoot == NULL)
    {
        printf("b+tree is null.\n");
        return false;
    }

    Node *node;

    node = m_pRoot;
    q.push(node);

    FILE *fp = fopen(path, "wb");
    while (q.empty() == false)
    {
        Node *nodeTmp = q.front();

        if (nodeTmp->m_bLeaf == false)
        {
            for (i = 0; i < nodeTmp->m_nkeynum; i++)
            {
                if (fwrite(nodeTmp, sizeof(InternalNode), 1, fp) != 1)
                { //写入内部节点
                    printf("write InternalNode Failed!\n");
                }
                else
                {
                    *internalCount++;
                }
                q.push((Node *)((InternalNode *)nodeTmp)->internalNodeData->m_pointers[i]);
            }
            q.push((Node *)((InternalNode *)nodeTmp)->internalNodeData->m_pointers[i]);
        }
        else
        {
            for (i = 0; i < nodeTmp->m_nkeynum; i++)
            {
                if (fwrite(nodeTmp, sizeof(LeafNode), 1, fp) != 1)
                { //写入叶子节点
                    printf("write LeafNode Failed!\n");
                }
                else
                {
                    *leafCount++;
                }
            }
        }
    }
    fclose(fp);
    return true;
}

Node *Deserialize(char *path, long long leafCount, long long internalCount)
{
    if (leafCount == 0)
        return NULL; //没有叶子节点，说明树为空
    Node *pRoot, *pTmp;
    FILE *fp = fopen(path, "rb");
    InternalNode inNodes[] = {};

    int i, j;
    if (internalCount > 0)
    {
        fread(inNodes, sizeof(InternalNode), internalCount, fp);

        pTmp = new Node;
        pTmp->m_nkeynum = inNodes[0].m_nkeynum;
        pTmp->m_pParent = inNodes[0].m_pParent;
        pTmp->m_bLeaf = inNodes[0].m_bLeaf;

        for (j = 0; j < inNodes[0].m_nkeynum; j++)
        {
            ((InternalNode *)pTmp)->internalNodeData->m_keys[j] = inNodes[0].internalNodeData->m_keys[j];
            ((InternalNode *)pTmp)->internalNodeData->m_pointers[j] = inNodes[0].internalNodeData->m_pointers[j];
        }
        pRoot = pTmp;

        for (i = 1; i < internalCount; i++)
        { //还原内部节点
            pTmp = new Node;
            pTmp->m_nkeynum = inNodes[i].m_nkeynum;
            pTmp->m_pParent = inNodes[i].m_pParent;
            pTmp->m_bLeaf = inNodes[i].m_bLeaf;
            for (j = 1; j < inNodes[i].m_nkeynum; j++)
            {
                ((InternalNode *)pTmp)->internalNodeData->m_keys[j] = inNodes[i].internalNodeData->m_keys[j];
                ((InternalNode *)pTmp)->internalNodeData->m_pointers[j] = inNodes[i].internalNodeData->m_pointers[j];
            }
        }
    }
    LeafNode leafNodes[] = {};

    fread(leafNodes, sizeof(leafNodes), leafCount, fp);
    for (i = 0; i < leafCount; i++)//还原叶子节点
    {
        pTmp = new Node;
        pTmp->m_bLeaf = leafNodes[i].m_bLeaf;
        pTmp->m_nkeynum=leafNodes[i].m_nkeynum;
        pTmp->m_pParent=leafNodes[i].m_pParent;
        for(j=0;j<pTmp->m_nkeynum;j++){
            ((LeafNode*)pTmp)->leafNodeData->m_keys[j]=leafNodes[i].leafNodeData->m_keys[j];
            ((LeafNode*)pTmp)->leafNodeData->m_number[j]=leafNodes[i].leafNodeData->m_number[j];
            ((LeafNode*)pTmp)->leafNodeData->next=leafNodes[i].leafNodeData->next;
        }
        if(internalCount==0&&leafCount>=0){//无内部节点，则只有一个根节点即当前叶子节点
            pRoot=pTmp;
        }
    }

    return pRoot;
}
