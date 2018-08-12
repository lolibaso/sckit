/*
 * https://en.wikipedia.org/wiki/Bloom_filter
 */
#ifndef __sc_bloomfilter_h
#define __sc_bloomfilter_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <sys/shm.h>

#define DEFAULT_BLOOMFILTER_SEED             0x1a2b3c4d  
#define DEFAULT_PROBABILITY_FALSE_POSITIVE   0.00001   // 十万分之一

namespace sc
{
class bloomfilter
{
public:
    bloomfilter();
    ~bloomfilter();

public:
    // 系统动态分配内存
    int init(const uint32_t max_items, 
             const double probability_false_positive = DEFAULT_PROBABILITY_FALSE_POSITIVE,
             const uint32_t seed = DEFAULT_BLOOMFILTER_SEED);     

    // 共享内存模式
    int init(key_t shm_key,
             const uint32_t max_items, 
             const double probability_false_positive = DEFAULT_PROBABILITY_FALSE_POSITIVE,
             const uint32_t seed = DEFAULT_BLOOMFILTER_SEED); 

    bool set(const void* key, const int len);
    bool check(const void* key, const int len);
    double usage_rate() const;
    void clear();

public:
    bool dump(const char* file);
    bool load(const char* file);

private:
    int  calc_param(const uint32_t n, const double p, uint32_t&m, uint32_t& k);
    void uninit();
    void bloomhash(const void* key, const int len);

private:
    bool            inited_;
    bool            use_shm_;     // 是否使用共享内存
    uint32_t        max_items_;   // the max number of bloomfilter, it's the  n  
    double          probability_false_positive_; // it's the p ,  the probability false positive, can be 0.001, 0.0001, 0.00001 ...
    uint32_t        seed_;        // the offset of the murmurhash' seed
    uint32_t        num_of_bits_; // 公式: m = ceil((n * log(p)) / log(1.0 / (pow(2.0, log(2.0))))); - bloomfilter的比特数
    uint32_t*       hash_pos_;        // the k array of hash_bits which the key hashed 
    uint32_t        num_of_hashfunc_; // 公式: k = round(log(2.0) * m / n); - 哈希函数个数 
    uint32_t        count_;   // the counts how many bits have been set 
    unsigned char*  entry_;   // the entry of bloomfilter 
    uint32_t        size_;    // the needed space size of bloomfilter 
};// end of class bloomfilter

} // end of namespace sc

#endif
