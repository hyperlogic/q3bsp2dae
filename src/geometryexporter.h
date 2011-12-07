#ifndef _GEOMETRYEXPORTER_
#define _GEOMETRYEXPORTER_

#include "COLLADASWLibraryGeometries.h"
#include "q3bsp.h"

class GeometryExporter : public COLLADASW::LibraryGeometries
{
public:
    GeometryExporter(COLLADASW::StreamWriter* sw, Q3BSP* bsp);
    ~GeometryExporter();
    void add();
protected:
    void appendSurface(std::string meshId, const dsurface_t* surface, const drawVert_t* verts, const int32_t* indices);
    void appendPatch(std::string meshId, const dsurface_t* surface, const drawVert_t* verts, const int32_t* indices);
    COLLADASW::StreamWriter* m_sw;
    Q3BSP* m_bsp;
};

#endif
