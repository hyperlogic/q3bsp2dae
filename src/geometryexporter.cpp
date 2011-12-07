#include "geometryexporter.h"
#include "q3bsp.h"
#include "quadpatch.h"
#include "COLLADASWSource.h"
#include "COLLADASWVertices.h"
#include "COLLADASWInputList.h"
#include "COLLADASWPrimitves.h"
#include <vector>

extern bool g_flipFaces;
extern bool g_flipTexCoords;

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

void GeometryExporter::appendSurface(std::string meshId, const dsurface_t* surface, const drawVert_t* verts, const int32_t* indices)
{
    // <mesh>
    openMesh(meshId);

    const int startVert = surface->firstVert;
    const int endVert = surface->numVerts + surface->firstVert;
    const int startIndex = surface->firstIndex;
    const int endIndex = surface->numIndices + surface->firstIndex;

    // positions <source>
    {
        COLLADASW::FloatSourceF fSrc(m_sw);
        fSrc.setId(meshId + POSITIONS_SOURCE_ID_SUFFIX);
        fSrc.setNodeName(meshId + POSITIONS_SOURCE_ID_SUFFIX);
        fSrc.setArrayId(meshId + POSITIONS_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX);
        fSrc.setAccessorStride(3);
        fSrc.setAccessorCount(surface->numVerts);
        fSrc.getParameterNameList().push_back("X");
        fSrc.getParameterNameList().push_back("Y");
        fSrc.getParameterNameList().push_back("Z");
        fSrc.prepareToAppendValues();
        for (int j = startVert; j < endVert; ++j)
            fSrc.appendValues(verts[j].xyz[0], verts[j].xyz[1], verts[j].xyz[2]);
        fSrc.finish();
    }

    // texcoords <source>
    {
        COLLADASW::FloatSourceF fSrc(m_sw);
        fSrc.setId(meshId + TEXCOORDS_SOURCE_ID_SUFFIX);
        fSrc.setNodeName(meshId + TEXCOORDS_SOURCE_ID_SUFFIX);
        fSrc.setArrayId(meshId + TEXCOORDS_SOURCE_ID_SUFFIX + ARRAY_ID_SUFFIX);
        fSrc.setAccessorStride(2);
        fSrc.setAccessorCount(surface->numVerts);
        fSrc.getParameterNameList().push_back("S");
        fSrc.getParameterNameList().push_back("T");
        fSrc.prepareToAppendValues();
        for (int j = startVert; j < endVert; ++j)
        {
            float t = verts[j].st[1];
            if (g_flipTexCoords)
                t = 1.0f - verts[j].st[1];
            fSrc.appendValues(verts[j].st[0], t);
        }
        fSrc.finish();
    }

    // lightmap texcoords <source>
    {
        COLLADASW::FloatSourceF fSrc(m_sw);
        fSrc.setId(meshId + TEXCOORDS_SOURCE_ID_SUFFIX + "1");
        fSrc.setNodeName(meshId + TEXCOORDS_SOURCE_ID_SUFFIX + "1");
        fSrc.setArrayId(meshId + TEXCOORDS_SOURCE_ID_SUFFIX + "1" + ARRAY_ID_SUFFIX);
        fSrc.setAccessorStride(2);
        fSrc.setAccessorCount(surface->numVerts);
        fSrc.getParameterNameList().push_back("S");
        fSrc.getParameterNameList().push_back("T");
        fSrc.prepareToAppendValues();
        for (int j = startVert; j < endVert; ++j)
        {
            float t = verts[j].lightmap[1];
            if (g_flipTexCoords)
                t = 1.0f - verts[j].lightmap[1];
            fSrc.appendValues(verts[j].lightmap[0], t);
        }
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
    triangles.appendCount((uint32_t)surface->numIndices);

    COLLADASW::InputList& triangleInputList = triangles.getInputList();
    triangleInputList.push_back(COLLADASW::Input(COLLADASW::InputSemantic::VERTEX,
                                                 COLLADASW::URI(EMPTY_STRING, meshId + VERTICES_ID_SUFFIX), 0));
    triangleInputList.push_back(COLLADASW::Input(COLLADASW::InputSemantic::TEXCOORD,
                                                 COLLADASW::URI(EMPTY_STRING, meshId + TEXCOORDS_SOURCE_ID_SUFFIX), 1));
    triangleInputList.push_back(COLLADASW::Input(COLLADASW::InputSemantic::TEXCOORD,
                                                 COLLADASW::URI(EMPTY_STRING, meshId + TEXCOORDS_SOURCE_ID_SUFFIX + "1"), 2));
    triangles.appendInputList();

    triangles.openPolylistElement();

    for (int j = startIndex; j < endIndex; ++j)
    {
        int index = indices[j];
        if (g_flipFaces)
            index = indices[startIndex + endIndex - j - 1];

        triangles.appendValues(index); // pos
        triangles.appendValues(index); // texcoord
        triangles.appendValues(index); // lightmap texcoord
    }
    triangles.finish();

    closeGeometry();
}

void GeometryExporter::appendPatch(std::string meshId, const dsurface_t* surface, const drawVert_t* verts, const int32_t* indices)
{
/*
    static int surfaceCount = 0;
    if (surfaceCount > 356)
        return;
    surfaceCount++;
    printf("PATCH meshId = %s\n", meshId.c_str());
*/

    QuadPatch patch;
    patch.setControlPoints(surface->patchWidth, surface->patchHeight, verts + surface->firstVert);
    patch.generateMesh(3);

    dsurface_t patchSurface;
    memcpy(&patchSurface, surface, sizeof(dsurface_t));
    patchSurface.firstVert = 0;
    patchSurface.numVerts = patch.getVerts().size();
    patchSurface.firstIndex = 0;
    patchSurface.numIndices = patch.getIndices().size();
    patchSurface.surfaceType = MST_TRIANGLE_SOUP;
    appendSurface(meshId, &patchSurface, &patch.getVerts()[0], &patch.getIndices()[0]);
}

void GeometryExporter::add()
{
    char tempStr[1024];

    openLibrary();
    for (int i = 0; i < m_bsp->numSurfaces; i++)
    {
        sprintf(tempStr, "surface%d", i);
        std::string meshId = tempStr;

        if (m_bsp->surfaces[i].surfaceType == MST_PATCH)
        {
            appendPatch(meshId, &m_bsp->surfaces[i], m_bsp->verts, m_bsp->indices);
        }
        else if (m_bsp->surfaces[i].surfaceType == MST_TRIANGLE_SOUP || m_bsp->surfaces[i].surfaceType == MST_PLANAR)
        {
            appendSurface(meshId, &m_bsp->surfaces[i], m_bsp->verts, m_bsp->indices);
        }
    }
    closeLibrary();
}
