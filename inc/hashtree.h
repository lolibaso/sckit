#ifndef __sc_hashtree_h
#define __sc_hashtree_h

/*
******************************************************************************
* Description: 哈希树 - 质数分辨定理实现
******************************************************************************
*/

namespace sc {

namespace ds {

template <class KeyType, class ValueType, int SIZE = 32>
class hashtree
{
    struct hashtree_node
    {
        hashtree_node() : occupied_(false), child_{0} {}
        KeyType key_;
        ValueType value_;
        bool occupied_;
        struct hashtree_node *child_[SIZE];
    };

public:
    hashtree() : max_level_(0), prime_{0}
    {
        for (int i = 2; i <= SIZE; ++i)
        {
            if (isprime(i))
            {
                prime_[++max_level_] = i;
            }
        }
    }

    /**
     * 插入函数
     * @param key 键
     * @param value 值
     * @return 成功 true, 失败 false
     */
    bool insert(const KeyType &key,  const ValueType &value)
    {
        return insert(&root_, 1, key, value);
    }

    /**
     * 查找函数
     * @param key 键
     * @return 指向value的指针, 找不到时为NULL
     */
    const ValueType *find(const KeyType &key)
    {
        return find(&root_, 1, key);
    }

    /**
     * 删除函数
     * @param key 键
     * @return 任何情况下都返回true
     */
    bool remove(const KeyType &key)
    {
        return remove(&root_, 1, key);
    }

private:
    bool insert(hashtree_node *node, int level, const KeyType &key, const ValueType &value)
    {
        if (!node->occupied_)
        {
            node->key_ = key;
            node->value_ = value;
            node->occupied_ = true;
            return true;
        }

        if (level > max_level_)
        {
            return false;
        }

        int idx = key % prime_[level];

        if (!node->child_[idx])
        {
            node->child_[idx] = new hashtree_node;
        }

        return insert(node->child_[idx], ++level, key, value);
    }

    const ValueType *find(hashtree_node *node, int level, const KeyType &key)
    {
        if (node->occupied_)
        {
            if (node->key_ == key)
            {
                return &node->value_;
            }
        }

        int idx = key % prime_[level];
        if (!node->child_[idx])
        {
            return 0;
        }

        return find(node->child_[idx], ++level, key);
    }

    bool remove(hashtree_node *node, int level, const KeyType &key)
    {
        if (node->occupied_)
        {
            if (node->key_ == key)
            {
                node->occupied_ = false;
                return true;
            }
        }

        int idx = key % prime_[level];
        if (!node->child_[idx])
        {
            return true;
        }

        return remove(node->child_[idx], ++level, key);
    }

    bool isprime(int n)
    {
        for (int i = 2; (i * i) < (n + 1); ++i)
        {
            if (!(n % i))
            {
                return false;
            }
        }
        return true;
    }

private:
    hashtree_node root_;
    int  max_level_;
    int  prime_[SIZE+1];
};

}  // namespace ds

} // namespace sc

#endif // end of __sc_hashtree_h 

#if 0
#include <iostream>
int main()
{
    sc::hashtree<int, std::string> tree;

    tree.insert(1111, "AAA");
    tree.insert(2222, "BBB");

    const std::string *value;

    value = tree.find(2222);
    std::cout << (value ? *value : "NULL") << std::endl;

    tree.remove(2222);

    value = tree.find(2222);
    std::cout << (value ? *value : "NULL") << std::endl;

    return 0;
}
#endif


