#include "imageexporter.h"
#include "q3bsp.h"
#include "COLLADASWImage.h"

ImageExporter::ImageExporter(COLLADASW::StreamWriter* sw, Q3BSP* bsp) :
    COLLADASW::LibraryImages(sw),
    m_sw(sw),
    m_bsp(bsp)
{
    ;
}

ImageExporter::~ImageExporter()
{
    ;
}

void ImageExporter::add()
{
    char tempStr[1024];

    openLibrary();
    for (int i = 0; i < m_bsp->numShaders; ++i)
    {
        sprintf(tempStr, "image%d", i);
        std::string imageId = tempStr;

        COLLADASW::URI sourceFileUri(COLLADASW::URI::nativePathToUri(m_bsp->shaders[i].shader));
        sourceFileUri.setScheme(COLLADASW::URI::SCHEME_FILE);
        COLLADASW::Image image(sourceFileUri, imageId);
        addImage(image);
    }
    closeLibrary();
}

