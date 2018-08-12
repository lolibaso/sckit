#include "test_comm.h"
#include "bloomfilter.h"
#include <iostream>
#include "mt_prng.h"

int main(int argc, char* argv[])
{
    sc::bloomfilter  bf;

    bf.init(0x1234,1000000);

    sc::mt_prng::seed(time(NULL));

    char s[100];
    for(int i=0; i<1000000; i++)
    {
        sc::mt_prng::rand_string(s, 16); 
        bf.set(s, 16);
    }

    std::cout << bf.usage_rate() << std::endl;
//    bf.clear();
//    std::cout << bf.usage_rate() << std::endl;

    for(int i=0; i<1000000; i++)
    {
        sc::mt_prng::rand_string(s, 16); 
        bf.set(s, 16);
    }

    bf.dump("./dumpfile");
    return 0;
}
