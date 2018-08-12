#ifndef __sc_math_util_h
#define __sc_math_util_h

#include <stdint.h>

namespace sc {

class math_util
{
public:
    static bool     is_pow2(uint64_t n);

    static uint8_t  get_pow2(uint64_t n);

    static uint64_t numofpow2(uint64_t num);

    static bool     is_prime(unsigned int n );

    /* return prime <= n */
    static uint32_t get_prime(uint32_t n);

    /* return prime >= n */
    static uint32_t get_prime2(uint32_t n);

    /* return primes[primes_num] <= max */
    static int      get_primes(uint32_t *primes, uint32_t primes_num, uint32_t max);

}; 

}  /* end of namespace sc */

#endif  /* end of __sc_math_util_h */
