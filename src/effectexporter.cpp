#include "effectexporter.h"
#include "q3bsp.h"
#include "COLLADASWEffectProfile.h"

EffectExporter::EffectExporter(COLLADASW::StreamWriter* sw, Q3BSP* bsp) :
    COLLADASW::LibraryEffects(sw),
    m_sw(sw),
    m_bsp(bsp)
{
    ;
}

EffectExporter::~EffectExporter()
{
    ;
}

void EffectExporter::add()
{
    char tempStr[1024];

    openLibrary();
    for (int i = 0; i < m_bsp->numShaders; ++i)
    {
        sprintf(tempStr, "material%d-fx", i);
        std::string fxId = tempStr;

        sprintf(tempStr, "sampler%d", i);
        std::string samplerId = tempStr;

        sprintf(tempStr, "surface%d", i);
        std::string surfaceId = tempStr;

        sprintf(tempStr, "image%d", i);
        std::string imageId = tempStr;

        openEffect(fxId);

        COLLADASW::EffectProfile effectProfile(m_sw);
        effectProfile.setProfileType(COLLADASW::EffectProfile::COMMON);

        effectProfile.setShaderType(COLLADASW::EffectProfile::CONSTANT);

        COLLADASW::Sampler sampler(COLLADASW::Sampler::SAMPLER_TYPE_2D, samplerId, surfaceId);
        sampler.setImageId(imageId);
        COLLADASW::Texture texture(samplerId);
        texture.setSampler(sampler);
        texture.setTexcoord("TEX0");

        effectProfile.setDiffuse(COLLADASW::ColorOrTexture(texture));

        effectProfile.openProfile();

        effectProfile.addSampler(COLLADASW::ColorOrTexture(texture));

        effectProfile.addProfileElements();

        effectProfile.closeProfile();

        closeEffect();
    }
    closeLibrary();
}

