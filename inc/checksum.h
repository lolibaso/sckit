#ifndef __sc_checksum_h
#define __sc_checksum_h
#include <stdint.h>

namespace sc
{
class checksum
{
public:
    static uint32_t get(const uint8_t* buf, int len);

    static bool     check(const uint8_t* buf, int len, uint32_t  cksum); 
};    
} 

#endif
