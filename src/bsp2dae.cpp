#include <stdio.h>
#include "q3bsp.h"

int main(int argc, const char* argv[])
{
    Q3BSP* bsp = Q3BSP_Load(argv[1]);
    Q3BSP_Free(bsp);
    return 0;
}
