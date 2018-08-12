#include "checksum.h"
#include "crc.h"

#define SC_CHECKSUM_MAGIC_NUM  0x7788aabb

namespace sc 
{

uint32_t 
checksum::get(const uint8_t* buf, int len)
{
    return crc::crc32(SC_CHECKSUM_MAGIC_NUM, buf, len);    
}

bool 
checksum::check(const uint8_t* buf, int len, uint32_t  cksum)
{
    return (cksum == get(buf, len));
}

}
