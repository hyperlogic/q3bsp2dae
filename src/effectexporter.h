#ifndef _EFFECTEXPORTER_
#define _EFFECTEXPORTER_

#include "COLLADASWLibraryEffects.h"

struct Q3BSP;

class EffectExporter : public COLLADASW::LibraryEffects
{
public:
    EffectExporter(COLLADASW::StreamWriter* sw, Q3BSP* bsp);
    ~EffectExporter();
    void add();
protected:
    COLLADASW::StreamWriter* m_sw;
    Q3BSP* m_bsp;
};

#endif
