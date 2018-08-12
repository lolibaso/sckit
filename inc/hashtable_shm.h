/*
 * 基于 shm 共享内存的2阶 hashtable 模版
 */

#ifndef __sc_hashtable_shm_h
#define __sc_hashtable_shm_h

#include <stdint.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "math_util.h"
#include "murmurhash3.h"

#define MIN_BUCKET_LEN   10000
#define DEF_BUCKET_NUM   10
#define MAX_BUCKET_NUM   50

namespace sc {

template <class KeyType>
struct _hash_to
{};

template <>
struct _hash_to<int32_t>
{
	uint32_t operator()(int32_t x) const { return (uint32_t)x; }
};

template <>
struct _hash_to<uint32_t>
{
	uint32_t operator()(uint32_t x) const { return x; }
};

template <>
struct _hash_to<int64_t>
{
	uint64_t operator()(int64_t x) const { return (uint64_t)x; }
};

template <>
struct _hash_to<uint64_t>
{
	uint64_t operator()(uint64_t x) const { return x; }
};

template <>
struct _hash_to<const char*>
{
	uint32_t operator()(const char* x) const
	{
		uint32_t h = 0;

        h = murmurhash3::hash32(x, strlen(x));

        return h;
	}
};

template <class _Arg1, class _Arg2, class _Result>
struct _bin_function
{
	typedef _Arg1  FirstArgType;
	typedef _Arg2  SecondArgType;
	typedef _Result ResultType;
};


template <class HashNode>
struct _equal_to : public _bin_function<HashNode, HashNode, bool>
{
    bool operator() (const HashNode& x, const HashNode& y) const { return x == y; }
};


template <>
struct _equal_to<int>
{
    bool operator() (int x, int y) const { return (x == y); }
};

template <>
struct _equal_to<uint32_t>
{
    bool operator() (uint32_t x, uint32_t y) const { return (x == y); }
};

template <>
struct _equal_to<int64_t>
{
    bool operator() (int64_t x, int64_t y) const { return (x == y); }
};


template <>
struct _equal_to<uint64_t>
{
    bool operator() (uint64_t x, uint64_t y) const { return (x == y); }
};

template <>
struct _equal_to<const char*>
{
    bool operator() (const char* x, const char* y) const { return (!strcmp(x , y)); }
};

template<class HashNode>
struct _is_empty
{};

template<>
struct _is_empty<int>
{
    bool operator() (int x) const { return x==0; }
};

template<>
struct _is_empty<uint32_t>
{
    bool operator() (uint32_t x) const { return x==0; }
};

template<>
struct _is_empty<int64_t>
{
    bool operator() (int64_t x) const { return x==0; }
};

template<>
struct _is_empty<uint64_t>
{
    bool operator() (uint64_t x) const { return x==0; }
};

template<>
struct _is_empty<const char*>
{
    bool operator() (const char* x) const { return *x==0x0; }
};


/*
 * 采用模板类，
 * HashNode 定义如下

template <class Key>
typedef struct _TestNode {
    KeyType   key;                // 这个是必须这样写。名字不能变，位置没关系
    // 其他数据
    // 其他数据
}TestNode;

 */

typedef struct HTShm_Head
{
	key_t   	shm_key;        // 共享内测 shm_key
    size_t      shm_size;       // 共享内存大小
    int         shm_id;

	int			bucket_num;     // 桶数,  最好不要超过50
	int 		bucket_len;     // 最大桶深

	int			count;          // 统计插入的个数
} HTShm_Head;

template<typename  KeyType,
         typename  HashNode,
         class     HashFunc = _hash_to<KeyType>,
         class     EqualFunc= _equal_to<KeyType>,
         class     EmptyFunc= _is_empty<KeyType> >
class hashtable_shm
{
    enum {
        HASHTABLE_SHM_MAGIC_SEED = 0x20180102
    };

public:
	hashtable_shm()
    {
        shm_ptr_    = NULL;
        shm_head_   = NULL;
        hash_table_ = NULL;
        primes_table_ = NULL;
        primes_table_sum_ = 0;
          
        memset(err_msg_, 0x0, sizeof(err_msg_));

        inited_ = false;
    }

	~hashtable_shm()
    {
        if(shm_ptr_)
        {
            shmdt(shm_ptr_);
            shm_ptr_ = NULL;
        }

        if(primes_table_)
        {
            free(primes_table_);
            primes_table_ = NULL;
        }
    }

    int init(const char* keyname, int bucket_num = DEF_BUCKET_NUM, int bucket_len = MIN_BUCKET_LEN)
    {
        key_t  k = murmurhash3::hash32(keyname, strlen(keyname), HASHTABLE_SHM_MAGIC_SEED);

        return init(k, bucket_num, bucket_len); 
    }

     /* bucket num is good  if it is less than 50 */
	int init(key_t shm_key, int bucket_num = DEF_BUCKET_NUM, int bucket_len = MIN_BUCKET_LEN)
    {
        size_t shm_size = 0;
        int    shm_id   = -1;

        if(inited_) return 0;

        if(bucket_len < MIN_BUCKET_LEN)  bucket_len = MIN_BUCKET_LEN;
        if(bucket_num > MAX_BUCKET_NUM)  bucket_num = MAX_BUCKET_NUM;
            
        // cal  primes table
        primes_table_ = (uint32_t*) malloc((bucket_num)*sizeof(uint32_t));
        if(!primes_table_)
        {
            sprintf(err_msg_, "memory alloc failed for primes_table_:%s", strerror(errno));
            return -1;
        }

        math_util::get_primes(primes_table_, bucket_num, bucket_len);
        for(int i=0; i<bucket_num; i++)
        {
            primes_table_sum_ += primes_table_[i] ;
        }
        shm_size = primes_table_sum_ * sizeof(HashNode) + sizeof(HTShm_Head);

        bool  new_created = false;
        shm_id = shmget(shm_key, shm_size, 0666);
        if(shm_id < 0)
        {
            // create new shm
            shm_id = shmget(shm_key, shm_size, IPC_CREAT |IPC_EXCL| 0666);
            if(shm_id < 0)
            {
                sprintf(err_msg_, "shmget %u %lu fail: %s", (uint32_t)shm_key, shm_size, strerror(errno));
                return -2;

            }
            new_created = true;
        }

        shm_ptr_ = shmat(shm_id, NULL, 0);
        if(shm_ptr_ == (void*)-1)
        {
            sprintf(err_msg_, "shmat fail: %d %s", shm_id, strerror(errno));
            return -3;
        }
        fprintf(stderr,"pos: %p id: %d size: %lu\n", shm_ptr_, shm_id, shm_size);

        shm_head_ = (HTShm_Head*) shm_ptr_;

        if(new_created) 
        {  
            memset(shm_ptr_, 0x0, shm_size);
            shm_head_->shm_key    = shm_key;
            shm_head_->shm_size   = shm_size;
            shm_head_->shm_id     = shm_id;
            shm_head_->bucket_num = bucket_num;
            shm_head_->bucket_len = bucket_len;
            shm_head_->count      = 0;
        }
        else
        {
            fprintf(stderr, "count: %d, %u  %lu  %d \n", 
                    shm_head_->count, 
                    (uint32_t)shm_head_->shm_key, shm_head_->shm_size, shm_head_->shm_id);
        }

        hash_table_ = (HashNode*) ((char*)shm_ptr_ + sizeof(HTShm_Head));

        hasher_     = HashFunc();
        equaler_    = EqualFunc();
        emptyer_    = EmptyFunc();
        inited_     = true;

        return 0;
    }

    /*
     *  插入时间平均 bucket_num 次 遍历
     */
   	bool insert(const KeyType& key, const HashNode* node , bool force=false)
    {
        uint32_t   index = 0;
        uint32_t   row   = 0;
        uint32_t   col   = 0;
        HashNode* p     = NULL;

        p  = find(key);
        if(p)
        {
           if(force)
           {
                memcpy(p, node, sizeof(*p));  // update
                return true;
           }
           else
           {
                sprintf(err_msg_, "key %u existed", node->key );
                return false;
           }
        }
        else
        {

            for(row=0; row<shm_head_->bucket_num; row++)
            {
                index = 0;
                col = hasher_(key) % primes_table_[row];
                for(uint32_t i=0; i<row; i++)
                {
                    index += primes_table_[i-1];
                }
                index += col;
                p = &(hash_table_[index]);

                if(emptyer_(p->key))
                {
                    memcpy(p, node, sizeof(*p));
                    shm_head_->count++;
                    return true;
                }
            }
            // 没找到位置
            sprintf(err_msg_,"no enough space to insert");
        }
        return false;
    }


    /*
     * 你必须保证这个数据只被插入一次, 这样保证数据不会频繁删除和插入导致hash空洞
     * eg：场景主要用于一次性导入数据进行，主要用于一次导入，永久查询的场景
     */
   	bool insert_onlyonce(const KeyType& key, const HashNode* node)
    {
        uint32_t   index = 0;
        uint32_t   row   = 0;
        uint32_t   col   = 0;
        HashNode*  p     = NULL;

        for(row=0; row<shm_head_->bucket_num; row++)
        {
            index = 0;
            col = hasher_(key) % primes_table_[row];
            for(uint32_t i=0; i<row; i++)
            {
                index += primes_table_[i-1];
            }

            index += col;
            p = &(hash_table_[index]);
            if(emptyer_(p->key))
            {
                memcpy(p, node, sizeof(*p));
                shm_head_->count++;
                return true;
            }
        }

        return false;
    }

    /*
     *  O(N)  N = shm_head_->bucket_num
     */
    HashNode* find(const KeyType&  key) const
    {
        uint32_t   index = 0;
        uint32_t   row   = 0;
        uint32_t   col   = 0;
        HashNode* p     = NULL;

        for(row=0; row<shm_head_->bucket_num; row++)
        {
            index = 0;
            col = hasher_(key) % primes_table_[row];
            for(int i=0; i<row; i++)
            {
                index += primes_table_[i-1];
            }
            index += col;
            p = &(hash_table_[index]);
            if(emptyer_(p->key))
                continue;

            if(equaler_(key, p->key))
            {
                return p;
            }
        }

        return NULL;
    }

	bool remove(const KeyType& key)
    {
        HashNode* node = find(key);
        if(NULL == node)
        {
            return false;
        }
        else
        {
            memset(node, 0x0, sizeof(*node));
            shm_head_->count--;
        }

        return true;
    }

    bool clear()
    {
        if(hash_table_ && shm_head_->shm_size>0)
        {
            memset(hash_table_, 0x0, shm_head_->shm_size);
        }

        return true;
    }

    bool empty() const
    {
        return (0 == shm_head_->count);
    }

    float usage_rate() const
    {
        return (float)shm_head_->count / primes_table_sum_ ;
    }

	int get_bucketnum() const
    {
        return  shm_head_->bucket_num;
    }

	uint32_t count() const
    {
        return shm_head_->count;
    }

    void print_primestable() const
    {
        fprintf(stderr, "\n");
        for(int i=0; i<shm_head_->bucket_num; i++)
        {
            fprintf(stderr, "%d ", primes_table_[i]);
        }
        fprintf(stderr, "\n");
    }

    void print_shmhead() const
    {
        if(NULL !=  shm_head_)
        {
           fprintf(stderr, "shm_key   :  %u  \n"  
                           "shm_id    :  %d   \n"
                           "shm_size  :  %u   \n"
                           "bucket_num:  %u   \n"
                           "bucket_len:  %u   \n"
                           "count     :  %u   \n",
                           (uint32_t)shm_head_->shm_key,
                           shm_head_->shm_id,
                           (uint32_t)shm_head_->shm_size,
                           shm_head_->bucket_num,
                           shm_head_->bucket_len,
                           shm_head_->count);
        }
    }

    const char* errmsg() const
    {
       return err_msg_;
    }

private:
    hashtable_shm(const hashtable_shm&);
    hashtable_shm &operator = (const hashtable_shm&);

private:
    bool        inited_;
    void    *   shm_ptr_;        // shm 头指针
    HTShm_Head* shm_head_;
	HashNode*	hash_table_;    // hash 表

    uint32_t*   primes_table_;   // 素数表
    uint32_t    primes_table_sum_; // 素数表所有素数总和



    HashFunc    hasher_;         // 用来 hash key 的函数
    EqualFunc   equaler_;        // 用来判断 key 值相等的函数
    EmptyFunc   emptyer_;        // 用来判断 key 是否为空的函数

    char        err_msg_[512];

};

}

#endif

