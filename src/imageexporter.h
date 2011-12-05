#ifndef _IMAGEEXPORTER_
#define _IMAGEEXPORTER_

#include "COLLADASWLibraryImages.h"

struct Q3BSP;

class ImageExporter : public COLLADASW::LibraryImages
{
public:
    ImageExporter(COLLADASW::StreamWriter* sw, Q3BSP* bsp);
    ~ImageExporter();
    void add();
protected:
    COLLADASW::StreamWriter* m_sw;
    Q3BSP* m_bsp;
};

#endif
