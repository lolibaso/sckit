#include "bloomfilter.h"
#include "murmurhash3.h"
#include "file_util.h"
#include "posix_shm.h"
#include <limits.h>

namespace sc
{


//#define SET_BIT(a, n) (a[n/CHAR_BIT] |= (1<<(n%CHAR_BIT)))
//#define GET_BIT(a, n) (a[n/CHAR_BIT] & (1<<(n%CHAR_BIT)))
#define  SET_BIT(n)  (entry_[(n)/CHAR_BIT] |= (1<<((n)%CHAR_BIT)))
#define  GET_BIT(n)  (entry_[(n)/CHAR_BIT] &  (1<<((n)%CHAR_BIT)))

/*
 *  100 seeds
 *  借用了 crc32 的表
 */
static const uint32_t g_seed_arrays[100] =
{
    0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
    0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
    0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
    0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
    0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
    0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
    0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
    0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
    0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
    0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
    0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L
};

bloomfilter::bloomfilter()
{
    inited_ = false;
    max_items_ = 0;
    probability_false_positive_ = DEFAULT_PROBABILITY_FALSE_POSITIVE;
    seed_ = DEFAULT_BLOOMFILTER_SEED;
    num_of_bits_ = 0;
    num_of_hashfunc_ = 0;
    count_ = 0;
    entry_ = NULL;
    size_  = 0;
    hash_pos_ = NULL;
    use_shm_ = false; 
}

bloomfilter::~bloomfilter()
{
    uninit();
}

int bloomfilter::calc_param(const uint32_t n, const double p, uint32_t&m, uint32_t& k)
{
    /*
     *  n - Number of items in the filter
     *  p - Probability of false positives, float between 0 and 1 or a number indicating 1-in-p
     *  m - Number of bits in the filter
     *  k - Number of hash functions
     *
     *  f = ln(2) × ln(1/2) × m / n = (0.6185) ^ (m/n)
     *  m = -1 * ln(p) × n / 0.6185
     *  k = ln(2) × m / n = 0.6931 * m / n
     */

    // 计算指定假阳概率下需要的比特数
    m = (uint32_t) ceil(-1 * log(p) * n / 0.6185);
    m = (m - m % 64) + 64;                  // 8字节对齐

    // 计算哈希函数个数
    k = (uint32_t) (0.6931 * m / n);
    k++;

    return 0;
}

int bloomfilter::init(uint32_t max_items, double probability_false_positive, uint32_t seed)
{
    if(inited_)  return 0;

    if(probability_false_positive <= 0 || probability_false_positive >= 1)
    {
        probability_false_positive = DEFAULT_PROBABILITY_FALSE_POSITIVE;
    }

    probability_false_positive_ = probability_false_positive;

    max_items_  = max_items;
    seed_       = seed;

    calc_param(max_items_, probability_false_positive_, num_of_bits_, num_of_hashfunc_);
    fprintf(stderr,"bits nums: %u   hashfunc nums: %d\n", num_of_bits_, num_of_hashfunc_);
    size_ = num_of_bits_ / CHAR_BIT ;
    entry_ = (unsigned char*) malloc(size_);
    if(NULL == entry_)
        return -1;

    hash_pos_ = (uint32_t*) malloc(num_of_hashfunc_ * sizeof(uint32_t));
    if(NULL == hash_pos_)
    {
        free(entry_);
        return -2;
    }

    memset(entry_, 0x0, size_);
    memset(hash_pos_, 0x0, num_of_hashfunc_ * sizeof(uint32_t));

    inited_ = true;

    return 0;
}

int bloomfilter::init(key_t shm_key, uint32_t max_items, double probability_false_positive, uint32_t seed)
{
    if(inited_)  return 0;

    if(probability_false_positive <= 0 || probability_false_positive >= 1)
    {
        probability_false_positive = DEFAULT_PROBABILITY_FALSE_POSITIVE;
    }

    probability_false_positive_ = probability_false_positive;

    max_items_  = max_items;
    seed_       = seed;

    calc_param(max_items_, probability_false_positive_, num_of_bits_, num_of_hashfunc_);
//   fprintf(stderr,"bits nums: %u   hashfunc nums: %d\n", num_of_bits_, num_of_hashfunc_);
    size_ = num_of_bits_ / CHAR_BIT ;

    sc::posix_shm  tmpshm(shm_key, size_);
    entry_ =  (unsigned char*)tmpshm.shm();
    if(NULL == entry_)
        return -1;

    for(uint32_t i=0; i < num_of_bits_; i++)
    {
        if(1 == GET_BIT(i))
            count_++;
    }

//   fprintf(stderr, "key: %u size: %lu\n", (uint32_t)tmpshm.key(), tmpshm.size());

    hash_pos_ = (uint32_t*) malloc(num_of_hashfunc_ * sizeof(uint32_t));
    if(NULL == hash_pos_)
    {
        free(entry_);
        return -2;
    }

//    memset(entry_, 0x0, size_);
    memset(hash_pos_, 0x0, num_of_hashfunc_ * sizeof(uint32_t));

    use_shm_ = true;
    inited_ = true;

    return 0;
}


void bloomfilter::bloomhash(const void* key ,int len)
{

    for(uint32_t i=0; i<num_of_hashfunc_; i++)
    {
        /* simply add two seed */
        uint32_t h = sc::murmurhash3::hash32(key, len, (seed_ + g_seed_arrays[i]));

        hash_pos_[i] = h % num_of_bits_;
    }

    return;
}

bool bloomfilter::set(const void* key, int len)
{
    if(NULL == key || len <= 0)
        return false;
    
    bloomhash(key, len);

    for(uint32_t i=0; i<num_of_hashfunc_; i++)
    {
        if( 0 == GET_BIT(hash_pos_[i]) )
        {
            SET_BIT(hash_pos_[i]);
            count_++;
        }
    }

    return true;
}

bool bloomfilter::check(const void* key, int len)
{
    if(NULL == key || len <= 0)
        return false;
   
    bloomhash(key, len);

    for(uint32_t i=0; i<num_of_hashfunc_; i++)
    {
        if( 0 == GET_BIT(hash_pos_[i]) )
            return false;
    }

    return true;
}

void bloomfilter::clear()
{
    memset(entry_, 0x0, size_); 
    count_ = 0;
}

void bloomfilter::uninit()
{
    if(NULL == entry_) 
        return ;

    if(!use_shm_)
    {
        ::free(entry_);
        entry_ = NULL;
    }

    ::free(hash_pos_);
    hash_pos_ = NULL;

    inited_ = false;
    count_  = 0;

    return;
}

double bloomfilter::usage_rate() const
{
    return ((double)count_ / num_of_bits_);
}

bool bloomfilter::load(const char* filename)
{    
    if(NULL == entry_ || size_ == 0)
        return false;

    if(0 >= sc::file_util::get_filecontent(filename, entry_, size_))
        return false;

    return true;
}

bool bloomfilter::dump(const char* filename)
{
    if(NULL == entry_ || size_ == 0)
        return false;

    if(0 >= sc::file_util::write_filecontent(filename, entry_, size_))
        return false;

    return true;
}

}
