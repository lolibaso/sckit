#include "test_comm.h"
#include "logger.h"

int main(int argc, char* argv[])
{
    sc::log_open("./test.log");

    int i = 0;
    while(1)
    {
        FatalLog("%d\n", i++); 
    }

    return 0;
}
