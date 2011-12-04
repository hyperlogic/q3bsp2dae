#include "geometryexporter.h"
#include "q3bsp.h"
#include "COLLADASWSource.h"

GeometryExporter::GeometryExporter(COLLADASW::StreamWriter* sw, Q3BSP* bsp) :
    COLLADASW::LibraryGeometries(sw),
    m_sw(sw),
    m_bsp(bsp)
{
    ;
}

GeometryExporter::~GeometryExporter()
{
    ;
}

void GeometryExporter::add()
{
    openLibrary();
    char tempStr[1024];
    for (int i = 0; i < /*m_bsp->numSurfaces*/3; i++)
    {
        sprintf(tempStr, "surface%d", i);
        std::string meshId = tempStr;
        openMesh(meshId);

        // positions
        {
            COLLADASW::FloatSourceF fSrc(m_sw);
            fSrc.setId(meshId + POSITIONS_SOURCE_ID_SUFFIX);
            fSrc.setNodeName(meshId + POSITIONS_SOURCE_ID_SUFFIX);
            fSrc.setArrayId(meshId + POSITIONS_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX);
            fSrc.setAccessorStride(3);
            fSrc.setAccessorCount(m_bsp->surfaces[i].numVerts);
            fSrc.getParameterNameList().push_back("X");
            fSrc.getParameterNameList().push_back("Y");
            fSrc.getParameterNameList().push_back("Z");
            fSrc.prepareToAppendValues();
            for (int j = m_bsp->surfaces[i].firstVert; j < m_bsp->surfaces[i].numVerts + m_bsp->surfaces[i].firstVert; ++j)
                fSrc.appendValues(m_bsp->verts[j].xyz[0], m_bsp->verts[j].xyz[1], m_bsp->verts[j].xyz[2]);
            fSrc.finish();
        }

        // uvs
        {
            COLLADASW::FloatSourceF fSrc(m_sw);
            fSrc.setId(meshId + TEXCOORDS_SOURCE_ID_SUFFIX);
            fSrc.setNodeName(meshId + TEXCOORDS_SOURCE_ID_SUFFIX);
            fSrc.setArrayId(meshId + TEXCOORDS_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX);
            fSrc.setAccessorStride(2);
            fSrc.setAccessorCount(m_bsp->surfaces[i].numVerts);
            fSrc.getParameterNameList().push_back("S");
            fSrc.getParameterNameList().push_back("T");
            fSrc.prepareToAppendValues();
            for (int j = m_bsp->surfaces[i].firstVert; j < m_bsp->surfaces[i].numVerts + m_bsp->surfaces[i].firstVert; ++j)
                fSrc.appendValues(m_bsp->verts[j].st[0], m_bsp->verts[j].st[1]);
            fSrc.finish();
        }

        // lightmap uvs
        {
            COLLADASW::FloatSourceF fSrc(m_sw);
            fSrc.setId(meshId + TEXCOORDS_SOURCE_ID_SUFFIX + "1");
            fSrc.setNodeName(meshId + TEXCOORDS_SOURCE_ID_SUFFIX + "1");
            fSrc.setArrayId(meshId + TEXCOORDS_SOURCE_ID_SUFFIX + "1" + ARRAY_ID_SUFFIX);
            fSrc.setAccessorStride(2);
            fSrc.setAccessorCount(m_bsp->surfaces[i].numVerts);
            fSrc.getParameterNameList().push_back("S");
            fSrc.getParameterNameList().push_back("T");
            fSrc.prepareToAppendValues();
            for (int j = m_bsp->surfaces[i].firstVert; j < m_bsp->surfaces[i].numVerts + m_bsp->surfaces[i].firstVert; ++j)
                fSrc.appendValues(m_bsp->verts[j].lightmap[0], m_bsp->verts[j].lightmap[1]);
            fSrc.finish();
        }

        // TODO: <vertices>
        // TODO: <triangles>

        closeGeometry();
    }
    closeLibrary();
}
