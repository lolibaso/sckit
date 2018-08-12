#ifndef __sc_murmurhash3_h
#define __sc_murmurhash3_h

#include <stdint.h>

namespace sc {

#define MURMURHASH3_MAGIC_SEED 0x1a2b3c4d

/*
 * Returns a murmur hash of `key' based on `seed' * using the MurmurHash3 algorithm
 *
 */

class murmurhash3
{
public:
    static uint32_t hash32( const void * key, const uint32_t len, const uint32_t seed = MURMURHASH3_MAGIC_SEED);

    static void     hash128( const void * key, const uint32_t len, void * out, const uint32_t seed = MURMURHASH3_MAGIC_SEED);

    static void     hash128_x64( const void * key, const uint32_t len, void * out, const uint32_t seed = MURMURHASH3_MAGIC_SEED);

}; 

} // end of namespace sc

#endif
