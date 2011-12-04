#ifndef _GEOMETRYEXPORTER_
#define _GEOMETRYEXPORTER_

#include "COLLADASWLibraryGeometries.h"

struct Q3BSP;

class GeometryExporter : public COLLADASW::LibraryGeometries
{
public:
    GeometryExporter(COLLADASW::StreamWriter* sw, Q3BSP* bsp);
    ~GeometryExporter();
    void add();
protected:
    COLLADASW::StreamWriter* m_sw;
    Q3BSP* m_bsp;
};

#endif
