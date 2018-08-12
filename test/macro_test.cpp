#include "test_comm.h"
#include "macro_func.h"

int main(int argc, char* argv[])
{
    printf("%d\n", MIN(1,2));
    printf("%d\n", MIN3(1,2, 3));
    printf("%d\n", MAX(1,-2));
    printf("%d\n", MAX3(1,-2,5));
    return 0;
}
