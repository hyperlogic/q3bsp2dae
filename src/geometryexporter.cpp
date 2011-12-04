#include "geometryexporter.h"
#include "q3bsp.h"
#include "COLLADASWSource.h"
#include "COLLADASWVertices.h"
#include "COLLADASWInputList.h"
#include "COLLADASWPrimitves.h"

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
    char tempStr[1024];

    openLibrary();
    for (int i = 0; i < m_bsp->numSurfaces; i++)
    {
        if (m_bsp->surfaces[i].surfaceType != MST_TRIANGLE_SOUP)
            continue;

        /*
        // TODO: REMOVE: for debugging only dump the first few surfaces
        static int dumpSurfaceCount = 0;
        dumpSurfaceCount++;
        if (dumpSurfaceCount > 3)
            break;
        */

        // <mesh>
        sprintf(tempStr, "surface%d", i);
        std::string meshId = tempStr;
        openMesh(meshId);

        // positions <source>
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

        // texcoords <source>
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

        // lightmap texcoords <source>
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

        // <vertices>
        COLLADASW::Vertices vertices(m_sw);
        vertices.setId(meshId + VERTICES_ID_SUFFIX);
        // <input semantic="POSITION">
        COLLADASW::InputList& inputList = vertices.getInputList();
        inputList.push_back(COLLADASW::Input(COLLADASW::InputSemantic::POSITION,
                                             COLLADASW::URI(EMPTY_STRING, meshId + POSITIONS_SOURCE_ID_SUFFIX)));
        vertices.add();

        // <triangles>
        COLLADASW::Triangles triangles(m_sw);
        triangles.openPrimitiveElement();
        triangles.appendMaterial("material");
        triangles.appendCount((uint32_t)m_bsp->surfaces[i].numIndices);

        COLLADASW::InputList& triangleInputList = triangles.getInputList();
        triangleInputList.push_back(COLLADASW::Input(COLLADASW::InputSemantic::VERTEX,
                                                     COLLADASW::URI(EMPTY_STRING, meshId + VERTICES_ID_SUFFIX), 0));
        triangleInputList.push_back(COLLADASW::Input(COLLADASW::InputSemantic::TEXCOORD,
                                                     COLLADASW::URI(EMPTY_STRING, meshId + TEXCOORDS_SOURCE_ID_SUFFIX), 1));
        triangleInputList.push_back(COLLADASW::Input(COLLADASW::InputSemantic::TEXCOORD,
                                                     COLLADASW::URI(EMPTY_STRING, meshId + TEXCOORDS_SOURCE_ID_SUFFIX + "1"), 2));
        triangles.appendInputList();

        triangles.openPolylistElement();
        for (int j = m_bsp->surfaces[i].firstIndex; j < m_bsp->surfaces[i].numIndices + m_bsp->surfaces[i].firstIndex; ++j)
        {
            int index = m_bsp->indices[j];
            triangles.appendValues(index); // pos
            triangles.appendValues(index); // texcoord
            triangles.appendValues(index); // lightmap texcoord
        }
        triangles.finish();

        closeGeometry();
    }
    closeLibrary();
}
