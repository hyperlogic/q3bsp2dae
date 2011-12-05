#include "materialexporter.h"
#include "q3bsp.h"

MaterialExporter::MaterialExporter(COLLADASW::StreamWriter* sw, Q3BSP* bsp) :
    COLLADASW::LibraryMaterials(sw),
    m_sw(sw),
    m_bsp(bsp)
{
    ;
}

MaterialExporter::~MaterialExporter()
{
    ;
}

void MaterialExporter::add()
{
    char tempStr[1024];

    openLibrary();
    for (int i = 0; i < m_bsp->numShaders; ++i)
    {
        sprintf(tempStr, "material%d", i);
        std::string matId = tempStr;

        sprintf(tempStr, "material%d-fx", i);
        std::string fxId = tempStr;

        openMaterial(matId, matId);
        addInstanceEffect(COLLADASW::URI(EMPTY_STRING, fxId));
        closeMaterial();
    }
    closeLibrary();
}

