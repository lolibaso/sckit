#include <stdint.h>
#include <math.h>
#include "math_util.h"

namespace sc {

/* is 2^n */
bool math_util::is_pow2(uint64_t n)
{
    return (n!=0) && ((n & (n-1))==0);
}

/* 返回一个不小于参数n的整数的关于2的幂次 */
uint8_t math_util::get_pow2(uint64_t n)
{
    uint8_t pow2 = 0;

    if(!is_pow2(n))
    {
       n = (n << 1);
    }

    while(n > 1)
    {
        pow2++;
        n = n >> 1;
    }

    return pow2;
}

/// 返回一个整数，其值不小于参数n，且为2的幂次方
uint64_t math_util::numofpow2(uint64_t n)
{
    if(is_pow2(n))
        return n;

    n = n-1;
    n = n | (n>>1);
    n = n | (n>>2);
    n = n | (n>>4);
    n = n | (n>>8);
    n = n | (n>>16);

    return n + 1;
}

bool math_util::is_prime(uint32_t n)
{
    uint32_t i, k;

    if(n==0 || n==1)
        return false;

    k = sqrt(n);

    for(i=2; i<=k; i++)
    {
        if(n%i == 0) break;
    }

    return (i > k);
}

uint32_t math_util::get_prime(uint32_t n)
{
    uint32_t i = 2;

    for(i=n; i>=2; i--)
    {
        if(is_prime(i))
            break;
    }

    return i;
}


int math_util::get_primes(uint32_t *primes, uint32_t primes_num, uint32_t max)
{
    uint32_t num = 0;

    for(int i=max; i>=2 && num<primes_num; i--) 
	{
        if(is_prime(i)) 
		{
            primes[num] = i;
            num++;
        }
    }

    return num;
}

/** 采用 6n+1 6n-1 素数集中原理 **/
/**
void GetPrimes(uint32_t *primes, uint32_t primes_num, uint32_t max)
{
    if (max< 5)
        return;

    unsigned long t, m, n, p;
    unsigned long x;
    int i, j, a, b, k;
    x = max;

    uint32_t z = 0;

    x = x / 6; 

    for (t = x; z < primes_num; t--)
    {
        i = 1; j = 1; k = t % 10;
        m = 6 * t;
        if (((k - 4) == 0) || ((k - 9) == 0) || ((m + 1) % 3 == 0))j = 0;
        if (((k - 6) == 0) || ((m - 1) % 3 == 0))i = 0;
        for (p = 1; p * 6 <= sqrt(m + 1) + 2; p++)
        {
            n = p * 6;
            k = p % 10;
            a = 1; b = 1;
            if (((k - 4) == 0) || ((k - 9) == 0))a = 0;
            if (((k - 6) == 0))b = 0;
            if (i){
                if (a){ if ((m - 1) % (n + 1) == 0)i = 0; }       
                if (b){ if ((m - 1) % (n - 1) == 0)i = 0; }
            }

            if (j){
                if (a){ if ((m + 1) % (n + 1) == 0)j = 0; } 
                if (b){ if ((m + 1) % (n - 1) == 0)j = 0; }
            }
            if ((i + j) == 0)break;
        }
        if (j){ primes[z] = m + 1; z++; if (z >= primes_num)break; }
        if (i){ primes[z] = m - 1; z++; if (z >= primes_num)break; }
    }

}
*/



} /* end of namespace sc */
#if 0
//
// g++ -o test_math_util math_util.cpp
//
#include <stdio.h>
int main(int argc, char* argv[])
{
    uint64_t x = sc::math_util::GetPow2(-1);
    uint64_t y = sc::math_util::NumOfPow2(140);
    printf("%ld %ld\n", x, y);

    return 0;
}
#endif
