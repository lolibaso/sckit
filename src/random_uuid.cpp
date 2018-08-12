#include "random_uuid.h"
#include "mt_prng.h"
#include <stdio.h>
#include <string.h>

/***
 * UUID 的格式为“xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx”，
 * 其中每个 x 是 0-9 或 a-f 范围内的一个4位十六进制数。
 * 例如：6F9619FF-8B86-D011-B42D-00C04FC964FF 即为有效的 UUID 值。
 **/
namespace sc {

static const int  kUUIDLength = 36U;  
static const char kHexChars[] = "0123456789ABCDEF";

bool random_uuid::is_valid(const char* uuid)
{
    if(!uuid)
        return false;

    if(strlen(uuid) != kUUIDLength)
        return false;

    for(int i=0; i<kUUIDLength; i++)
    {
        if(i==8 || i==13 || i==18 || i==23)
        {
            if(uuid[i] != '-')
                return false;
        }
        else
        {
            if(NULL == strchr(kHexChars, uuid[i]))
                return false;
        }
    }

    return true;
}

void random_uuid::seed(uint32_t seed_num)
{
    mt_prng::seed(seed_num);
}

char* random_uuid::generate(char* uuid)
{
    mt_prng::rand_string(uuid, (8+1+4+1+4+1+4+1+12), kHexChars); 

    uuid[8]  = '-';
    uuid[13] = '-';
    uuid[18] = '-';
    uuid[23] = '-';
 
    return uuid;
}

} // end of namespace sc 

#if 0
#include <stdio.h>
int main()
{
    char uuid[100] ={0}; 

    sc::random_uuid::seed(12345);

    for(int i=0; i<10; i++)
    {
        sc::random_uuid::generate(uuid);

        printf("%s\n", uuid);
    }

    return 0;
}
#endif

