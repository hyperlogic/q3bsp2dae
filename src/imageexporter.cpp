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

static bool _FileExists(const std::string& filename)
{
    FILE* fp = fopen(filename.c_str(), "rb");
    if (fp)
    {
        fclose(fp);
        return true;
    }
    return false;
}

void ImageExporter::add()
{
    char tempStr[1024];

    openLibrary();
    for (int i = 0; i < m_bsp->numShaders; ++i)
    {
        sprintf(tempStr, "image%d", i);
        std::string imageId = tempStr;

        // Check for existence of .jpg or .tga texture.
        std::string shader = std::string(m_bsp->shaders[i].shader);
        std::string imageFile = shader + ".jpg";
        if (!_FileExists(imageFile))
            imageFile = shader + ".tga";

        COLLADASW::Image image(imageFile, imageId);
        addImage(image);
    }
    closeLibrary();
}

