#ifndef __sc_random_uuid_h
#define __sc_random_uuid_h
#include <stdint.h>

namespace sc {

class random_uuid
{
public:
    static void  seed(uint32_t seed_num);

    /* uuid must has enough size , > 36 bytes */
    static char* generate(char* uuid);

    static bool  is_valid(const char* uuid);
}; 

} // end of namespace sc 

#endif
