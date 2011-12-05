#ifndef _MATERIALEXPORTER_
#define _MATERIALEXPORTER_

#include "COLLADASWLibraryMaterials.h"

struct Q3BSP;

class MaterialExporter : public COLLADASW::LibraryMaterials
{
public:
    MaterialExporter(COLLADASW::StreamWriter* sw, Q3BSP* bsp);
    ~MaterialExporter();
    void add();
protected:
    COLLADASW::StreamWriter* m_sw;
    Q3BSP* m_bsp;
};

#endif
