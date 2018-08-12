#ifndef __DATRIE_H__
#define __DATRIE_H__

/**  
* @file     DTrie.h
* @brief    动态Trie树的实现，实现简单的前缀、后缀、精确检索
*           后缀的检索用ReTrie来创建
*  
* @author   kiddyhuang          
*/ 

#include <cstring>
#include <string>
#include <vector>
#include <cassert>
#include <queue>
#include <climits>

using namespace std;

class EmptyType {};
    
template<typename T>
class DTrie
{
public:

    #pragma pack(1)
    /**
    * @brief Node结点
    *   pIndex 指向索引数据的起始地址
    *   size   记录了指向的Index数组的长度 
    *   status 表示当前结点的状态，-1表示非终止状态，其余均表示终态（可以作为分类依据）。
    */
    
    struct Node
    {
        unsigned int    pIndex; 
        char            status; //-1: uncomplete  
        unsigned char   size;
        T               value;
    };


    /**
    * @brief Index结点
    *   pNode   指向Node结点地址
    *   ch      表示当前结点索引的字符。
    */
    struct Index 
    {
        unsigned int    pNode;
        char            ch; 
    };
    #pragma pack()

    struct TData
    {
        string          sKey;
        char            status;
        T               value;
    };

    /**
    * @brief 内存控制块，记录内存的使用情况
    */
    struct HEAD_BLOCK
    {
        size_t          iNodeMemSize;
        size_t          iIndexMemSize;
        unsigned int    iNodeOffset;
        unsigned int    iIndexOffset;
    };

    /**
    * @brief 构造函数
    */
    DTrie();
    virtual ~DTrie(){};

    /**
     *  @brief 创建Trie树
     *  
     *  @param pNodeAddr        提供Node的数据地址
     *  @param iNodeMemSize     Node地址内存大小
     *  @param pIndexAddr       提供Index的数据地址
     *  @param iIndexMemSize    Index地址内存大小
     *
     *  @return int 
     */
    int create(void* pNodeAddr, size_t iNodeMemSize, void* pIndexAddr, size_t iIndexMemSize);

    /**
     *  @brief 连接上指定地址的Trie树
     *  
     *  @param pNodeAddr        提供Node的数据地址
     *  @param pIndexAddr       提供Index的数据地址
     *
     *  @return int 
     */
    int connect(void* pNodeAddr, void* pIndexAddr);


    /**
     *  @brief 前缀检索
     *  @param sKey             待检索的串
     *  @param iSameNum         指定与待检索串前缀匹配的位数 
     *  @param vecData          存储检索结果变量
     *  @param iMaxNum          限制最多的结果数
     *  @return int 
     */
    virtual int find        (const string& sKey, unsigned int iSameNum, vector<DTrie::TData>& vecData, size_t iMaxNum = 5000);

    /**
     *  @brief 精确检索
     */
    virtual int find        (const string& sKey, TData& stResult);

    /**
     *  @brief  删除对应Key，线程安全
     */
    virtual int del         (const string& sKey);


    /**
     *  @brief  添加对应Key和相应的状态，非线程安全
     */
    virtual int add         (const TData& tData);

protected:
    int preOrder(Node* pNode, string curKey, vector<TData>& stResult, size_t iMaxNum);
    virtual Node* find      (const string& sKey);

    inline Node*    PTR         (size_t offset);
    inline Index*   PTR_INDEX   (size_t offset);
    inline Node*    INIT_NODE   (size_t offset);
    inline bool     INITED();

    inline Node* GetNextNode   (Node* pCur, char index);
    //inline Node GetNextNode(Node pCur, char index);
    inline Node* AddChild      (Node* pCur, char index);


protected:

    /**
    * @brief 内存池子队列，存放空闲可以利用的内存空间
    *        存相应下标大小的Index空闲空间首地址
    */
    queue<unsigned int> m_IndexPool[ UCHAR_MAX+1 ];

    HEAD_BLOCK*         m_pHeadBlock;

    Node*               m_pNodeRoot;
    Index*              m_pIndexRoot;

    size_t              m_iMaxNode;
    size_t              m_iMaxIndex;

};




template<typename T>
DTrie<T>::DTrie()
{
    m_pHeadBlock    = NULL;
    m_pNodeRoot     = NULL;
    m_pIndexRoot    = NULL;
}

template<typename T>
int DTrie<T>::create(void* pNodeAddr, size_t iNodeMemSize, void* pIndexAddr, size_t iIndexMemSize)
{
    m_pHeadBlock    = (HEAD_BLOCK*) pNodeAddr;

    m_pHeadBlock->iNodeMemSize = iNodeMemSize;
    m_pHeadBlock->iIndexMemSize = iIndexMemSize;
    m_iMaxNode      = (iNodeMemSize - sizeof(HEAD_BLOCK))/sizeof(Node);
    m_iMaxIndex     = iIndexMemSize/sizeof(Index);

    m_pNodeRoot     = (Node*) ((char*)pNodeAddr + sizeof(HEAD_BLOCK));
    m_pIndexRoot    = (Index*) pIndexAddr;


    //初始化根Node
    INIT_NODE(0);

    m_pHeadBlock->iNodeOffset   = 1;
    m_pHeadBlock->iIndexOffset  = 1;

    return 0;
}

template<typename T>
int DTrie<T>::connect(void* pNodeAddr, void* pIndexAddr)
{
    m_pHeadBlock    = (HEAD_BLOCK*) pNodeAddr;

    m_pNodeRoot     = (Node*) ((char*)pNodeAddr + sizeof(HEAD_BLOCK));
    m_pIndexRoot    = (Index*) pIndexAddr;

    return 0;
}


template<typename T>
int DTrie<T>::add(const DTrie<T>::TData& tData)
{
    if(!INITED())
        return -1;

    Node* pCurNode = m_pNodeRoot;
    Node* pNext = NULL;
    const string& sKey = tData.sKey;
    
    char ch;

    for(size_t i=0; i<sKey.length(); )
    {
        ch = sKey[i];
        pNext = GetNextNode(pCurNode, ch);

        if(pNext == NULL) //没有相应的子结点
            pCurNode = AddChild(pCurNode, ch);
        else
            pCurNode = pNext;


        if(pCurNode == NULL)
            return -2; 

        if(++i == sKey.length()) //叶子结点
        {
            pCurNode->status = tData.status;

            if(sizeof(pCurNode->value) != 0)
                *(T*)pCurNode->value = tData.value;
        }

    }

    return 0;
}




template<typename T>
typename DTrie<T>::Node* DTrie<T>::GetNextNode(DTrie<T>::Node* pCur, char index)
{
    if(pCur == NULL || pCur->size == 0)
        return NULL;


    Index* pIndex = PTR_INDEX(pCur->pIndex);

    for(unsigned int i=0; i<pCur->size; i++)
    {
        if(pIndex->ch == index)
            return PTR(pIndex->pNode);

        pIndex++;
    }
    return NULL;
}


template<typename T>
typename DTrie<T>::Node* DTrie<T>::AddChild(DTrie<T>::Node* pCur, char index)
{
    if(m_pHeadBlock->iNodeOffset+1 > m_iMaxNode)
        return NULL; 
    
    //将当前的Index数组放回对应的池中
    if(pCur->size > 0 && pCur->pIndex != 0)
        m_IndexPool[(size_t)pCur->size].push(pCur->pIndex);
    

    size_t iNextIndex = m_pHeadBlock->iIndexOffset;
    bool bIsPool = false;

    //从池子取出
    if(!m_IndexPool[pCur->size + 1].empty())
    {
        iNextIndex = m_IndexPool[pCur->size + 1].front();
        m_IndexPool[pCur->size + 1].pop();
        bIsPool = true;
    }
    else if(m_pHeadBlock->iIndexOffset + (pCur->size + 1) > m_iMaxIndex)
        return NULL;

    Index* pOldIndex = PTR_INDEX(pCur->pIndex);
    Index* pNewIndex = PTR_INDEX(iNextIndex);


    //拷贝老数据到新的Index数组
    memcpy(pNewIndex, pOldIndex, sizeof(Index)*pCur->size);
    pNewIndex += pCur->size;

    if(!bIsPool)
        m_pHeadBlock->iIndexOffset += pCur->size;

    pCur->size = pCur->size + 1;
    pCur->pIndex = iNextIndex;



    pNewIndex->ch = index;
    pNewIndex->pNode = m_pHeadBlock->iNodeOffset;

    Node* pNewNode = INIT_NODE(pNewIndex->pNode);


    if(!bIsPool)
        m_pHeadBlock->iIndexOffset++;

    m_pHeadBlock->iNodeOffset++;


    return pNewNode;

}

template<typename T>
int DTrie<T>::find(const string& sKey, TData& stTData)
{
    if(!INITED())
        return -1;

    Node* pCurNode = find(sKey);

    if(pCurNode == NULL || pCurNode->status == -1)
        return -2;
    else
    {
        stTData.sKey = sKey;
        stTData.status = pCurNode->status;
        if(sizeof(pCurNode->value) != 0)
            stTData.value = *(T*)pCurNode->value;
        return 0;
    }

    return -2;
}


template<typename T>
int DTrie<T>::find(const string& sKey, unsigned int iSameNum, vector<DTrie::TData>& vecData, size_t iMaxNum)
{
    if(!INITED())
        return -1;

    Node* pCurNode = find(sKey.substr(0, iSameNum));
    vecData.clear();

    if(pCurNode == NULL)
        return -2;

    preOrder(pCurNode, sKey.substr(0, iSameNum), vecData, iMaxNum);

    if(vecData.size() > 0)
        return 0;


    return -2;
}

template<typename T>
int DTrie<T>::preOrder(Node* pNode, string curKey, vector<TData>& stResult, size_t iMaxNum)
{
    if (pNode->status >= 0)
    {
        TData data;

        data.status = pNode->status;
        data.sKey = curKey;

        if(sizeof(pNode->value) != 0)
            data.value =* (T*)pNode->value;
        stResult.push_back(data);

        if(stResult.size() >= iMaxNum)
            return 0;
    }

    for (unsigned int i = 0; i < pNode->size; i++)
    {
        preOrder(PTR(PTR_INDEX(pNode->pIndex + i)->pNode), 
                curKey + string(1, PTR_INDEX(pNode->pIndex + i)->ch), 
                stResult, 
                iMaxNum);
    }

    return 0;
}

template<typename T>
int DTrie<T>::del(const string& sKey)
{
    if(!INITED())
        return -1;

    Node* pNode = find(sKey);

    if(pNode == NULL || pNode->status == -1)
        return -2;

    pNode->status = -1;

    return 0;
}

template<typename T>
typename DTrie<T>::Node* DTrie<T>::find(const string& sKey)
{
    if(!INITED())
        return NULL;

    Node* pCurNode = m_pNodeRoot;
    for(size_t i=0; i<sKey.length(); i++)
    {
        pCurNode = GetNextNode(pCurNode, sKey[i]);
        if(pCurNode == NULL) //找不到对应的child指针
            return NULL;
    }

    return pCurNode;
}

template<typename T>
inline bool DTrie<T>::INITED()
{
    return (m_pHeadBlock !=NULL && m_pIndexRoot != NULL && m_pNodeRoot != NULL);
}

template<typename T>
typename DTrie<T>::Node* DTrie<T>::PTR(size_t offset)
{
    return m_pNodeRoot  + offset;
}


template<typename T>
inline typename DTrie<T>::Index* DTrie<T>::PTR_INDEX(size_t offset)
{
    return m_pIndexRoot + offset;
}

template<typename T>
inline typename DTrie<T>::Node* DTrie<T>::INIT_NODE(size_t offset)
{
    PTR(offset)->status = -1;
    PTR(offset)->size = 0;
    PTR(offset)->pIndex = 0;
    return PTR(offset);
}


//采用模板特化，将空类型的模板，优化Node大小，
//从占8字节到6字节优化
#pragma pack(1)
template<>
struct DTrie<EmptyType>::Node
{
    unsigned int    pIndex; 
    char            status; //-1: uncomplete  
    unsigned char   size;
    EmptyType       value[0];
};
#pragma pack()

#endif

