#ifndef _SCENEEXPORTER_
#define _SCENEEXPORTER_

#include "COLLADASWLibraryVisualScenes.h"

struct Q3BSP;

class SceneExporter : public COLLADASW::LibraryVisualScenes
{
public:
    SceneExporter(COLLADASW::StreamWriter* sw, Q3BSP* bsp);
    ~SceneExporter();
    void add();
protected:
    COLLADASW::StreamWriter* m_sw;
    Q3BSP* m_bsp;
};

#endif
