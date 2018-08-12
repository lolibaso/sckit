#include "test_comm.h"
#include "wheel_timer.h"
#include "time_util.h"
#include <iostream>

using namespace sc;

wheel_timer t;

static void foo(uint32_t ii)
{
    std::cout << "-------------\n";
    std::cout << ii << std::endl;
    std::cout << t.size() << std::endl;
}

int main(int argc, char* argv[])
{
    time_t  tt = time_util::now();
    int i = 0;

    
    t.add_timer(1000, std::bind(&foo, tt));
    t.add_timer(2000, std::bind(&foo, tt));
    t.add_timer(20000, std::bind(&foo, tt));

    std::cout << t.size() << std::endl;

    while(t.size() > 0)
    {
        t.update();

        usleep(1000);
    }
    
    
    return 0;
}
