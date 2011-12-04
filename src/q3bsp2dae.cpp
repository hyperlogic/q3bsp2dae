#include <stdio.h>
#include "q3bsp.h"
#include "COLLADASWStreamWriter.h"

int main(int argc, const char* argv[])
{
    Q3BSP* bsp = Q3BSP_Load(argv[1]);

    if (argc != 3)
    {
        printf("Usage: q3bsp2dae q3dm6.bsp q3dm6.dae\n");
        return 1;
    }

    std::string outFile = std::string(argv[2]);
    COLLADASW::StreamWriter* sw = new COLLADASW::StreamWriter(COLLADASW::NativeString(outFile));
    sw->startDocument();
    delete sw;

    Q3BSP_Free(bsp);
    return 0;
}
