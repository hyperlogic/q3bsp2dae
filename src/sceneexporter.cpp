#include "sceneexporter.h"
#include "q3bsp.h"
#include "COLLADASWNode.h"
#include "COLLADASWInstanceGeometry.h"
#include "COLLADASWBindMaterial.h"
#include "COLLADASWInstanceMaterial.h"

SceneExporter::SceneExporter(COLLADASW::StreamWriter* sw, Q3BSP* bsp) :
    COLLADASW::LibraryVisualScenes(sw),
    m_sw(sw),
    m_bsp(bsp)
{
    ;
}

SceneExporter::~SceneExporter()
{

}

void SceneExporter::add()
{
    char tempStr[1024];

    openLibrary();
    openVisualScene("root");
    for (int i = 0; i < m_bsp->numSurfaces; i++)
    {
        sprintf(tempStr, "node%d", i);
        std::string nodeId = tempStr;

        sprintf(tempStr, "surface%d", i);
        std::string meshId = tempStr;

        sprintf(tempStr, "material%d", m_bsp->surfaces[i].shaderNum);
        std::string matId = tempStr;

        COLLADASW::Node node(m_sw);
        node.setNodeId(nodeId);
        node.setType(COLLADASW::Node::NODE);
        node.start();

        COLLADASW::InstanceGeometry instanceGeom(m_sw);
        instanceGeom.setUrl(COLLADASW::URI(EMPTY_STRING, meshId));
        COLLADASW::BindMaterial& bindMat = instanceGeom.getBindMaterial();
        COLLADASW::InstanceMaterialList& instanceMatList = bindMat.getInstanceMaterialList();
        instanceMatList.push_back(COLLADASW::InstanceMaterial("material", matId));
        instanceGeom.add();
        node.end();
    }
    closeVisualScene();
    closeLibrary();
}
