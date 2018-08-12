#include "test_comm.h"
#include "hashtable_shm.h"

typedef struct TestNode
{
    int  key; 
    int  count;
} TestNode;

int main(int argc, char* argv[])
{
    sc::hashtable_shm<int , TestNode> htshm;
    htshm.init("test");

    TestNode node {1, 1};
    if(false == htshm.insert(1, &node))
        printf(" error: %s \n", htshm.errmsg());
    
    printf("%d items\n", htshm.count());
    printf("%f full rate\n", htshm.usage_rate());
    htshm.print_primestable();
    htshm.print_shmhead();
    printf("\n");

    //while(1)
    {
        sleep(20);
    }

    return 0;
}
