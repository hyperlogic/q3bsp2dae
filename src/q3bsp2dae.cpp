#include <stdio.h>
#include "q3bsp.h"
#include "COLLADASWStreamWriter.h"
#include "COLLADASWAsset.h"
#include "geometryexporter.h"

static void _DumpAsset(Q3BSP* bsp, COLLADASW::StreamWriter* sw, const std::string& inFile)
{
    COLLADASW::Asset asset(sw);
    asset.getContributor().mAuthoringTool = "q3bsp2dae";
    asset.getContributor().mSourceData = inFile;
    asset.setUpAxisType(COLLADASW::Asset::Y_UP);  // true?
    asset.add();
}

static void _DumpGeometries(Q3BSP* bsp, COLLADASW::StreamWriter* sw)
{
    GeometryExporter geometryExporter(sw, bsp);
    geometryExporter.add();
}

int main(int argc, const char* argv[])
{
    Q3BSP* bsp = Q3BSP_Load(argv[1]);

    if (argc != 3)
    {
        printf("Usage: q3bsp2dae q3dm6.bsp q3dm6.dae\n");
        return 1;
    }

    std::string inFile = std::string(argv[1]);
    std::string outFile = std::string(argv[2]);
    COLLADASW::StreamWriter* sw = new COLLADASW::StreamWriter(COLLADASW::NativeString(outFile));
    sw->startDocument();

    _DumpAsset(bsp, sw, inFile);
    _DumpGeometries(bsp, sw);

    delete sw;

    Q3BSP_Free(bsp);
    return 0;
}
