#include "q3bsp.h"
#include <stdio.h>
#include <string.h>

enum LoadFileToMemoryResult { CouldNotOpenFile = -1, CouldNotReadFile = -2 };
static int _LoadFileToMemory(const char *filename, char **result)
{
    int size = 0;
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
    {
        *result = NULL;
        return CouldNotOpenFile;
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    *result = new char[size + 1];

    int newSize = (int)fread(*result, sizeof(char), size, f);
    if (size != newSize)
    {
        fprintf(stderr, "size mismatch, size = %d, newSize = %d\n", size, newSize);
        delete [] *result;
        return CouldNotReadFile;
    }
    fclose(f);
    (*result)[size] = 0;  // make sure it's null terminated.
    return size;
}

static void _LoadShaders(Q3BSP* bsp, char* buffer, lump_t* lump)
{
    dshader_t* in = (dshader_t*)(buffer + lump->fileofs);
    if (lump->filelen % sizeof(dshader_t))
    {
        fprintf(stderr, "Q3BSP_Load: bad shader lump size!\n");
        return;
    }
    int count = lump->filelen / sizeof(dshader_t);
    dshader_t* out = new dshader_t[count];
    memcpy(out, in, count * sizeof(dshader_t));

    // swap flags
    for (int i = 0; i < count; ++i)
    {
        out[i].surfaceFlags = LittleLong(out[i].surfaceFlags);
        out[i].contentFlags = LittleLong(out[i].surfaceFlags);
    }

    bsp->shaders = out;
    bsp->numShaders = count;

    /*
    // dump shaders
    printf("shaders = \n");
    for (int i = 0; i < count; ++i)
    {
        printf("    path = \"%s\", surfaceFlags = 0x%x, contentFlags = 0x%x\n", out[i].shader, out[i].surfaceFlags, out[i].contentFlags);
    }
    */
    printf("numShaders = %d\n", bsp->numShaders);
}

static void _LoadLightmaps(Q3BSP* bsp, char* buffer, lump_t* lump)
{
    int32_t len = lump->filelen;
    if (!len)
        return;
    char* buff = buffer + lump->fileofs;
    size_t size = (LIGHTMAP_SIZE * LIGHTMAP_SIZE * 3);
    bsp->numLightmaps = len / size;
    bsp->lightmaps = new dlightmap_t[bsp->numLightmaps];
    for (int i = 0; i < bsp->numLightmaps; ++i)
    {
        bsp->lightmaps[i].width = LIGHTMAP_SIZE;
        bsp->lightmaps[i].height = LIGHTMAP_SIZE;
        bsp->lightmaps[i].data = new unsigned char[size];
        memcpy(bsp->lightmaps[i].data, buff + size * i, size);
    }

    printf("numLightmaps = %d\n", bsp->numLightmaps);
}

static void _LoadSurfaces(Q3BSP* bsp, char* buffer, lump_t* lump)
{
    dsurface_t* in = (dsurface_t*)(buffer + lump->fileofs);
    if (lump->filelen % sizeof(dsurface_t))
    {
        fprintf(stderr, "Q3BSP_Load: bad surface lump size!\n");
        return;
    }
    int count = lump->filelen / sizeof(dsurface_t);
    dsurface_t* out = new dsurface_t[count];
    memcpy(out, in, sizeof(dsurface_t) * count);

#define SL(field) out[i].field = LittleLong(out[i].field)
#define SV3(field) do {                                 \
        out[i].field[0] = LittleFloat(out[i].field[0]); \
        out[i].field[1] = LittleFloat(out[i].field[1]); \
        out[i].field[2] = LittleFloat(out[i].field[2]); \
    } while(0)

    // byte swap
    for (int i = 0; i < count; ++i)
    {
        SL(shaderNum); SL(fogNum); SL(surfaceType); SL(firstVert);
        SL(numVerts); SL(firstIndex); SL(numIndexes); SL(lightmapNum);
        SL(lightmapX); SL(lightmapY); SL(lightmapWidth); SL(lightmapHeight);
        SV3(lightmapOrigin); SV3(lightmapVecs[0]); SV3(lightmapVecs[1]); SV3(lightmapVecs[2]);
        SL(patchWidth); SL(patchHeight);
    }
#undef SL
#undef SV3

    bsp->surfaces = out;
    bsp->numSurfaces = count;

    printf("numSurfaces = %d\n", bsp->numSurfaces);
}

static void _LoadVertices(Q3BSP* bsp, char* buffer, lump_t* lump)
{
    drawVert_t* in = (drawVert_t*)(buffer + lump->fileofs);
    if (lump->filelen % sizeof(drawVert_t))
    {
        fprintf(stderr, "Q3BSP_Load: bad drawVert lump size!\n");
        return;
    }
    int count = lump->filelen / sizeof(drawVert_t);
    drawVert_t* out = new drawVert_t[count];
    memcpy(out, in, sizeof(drawVert_t) * count);

#define SF(field) out[i].field = LittleFloat(out[i].field)
#define SV3(field) do {                                 \
        out[i].field[0] = LittleFloat(out[i].field[0]); \
        out[i].field[1] = LittleFloat(out[i].field[1]); \
        out[i].field[2] = LittleFloat(out[i].field[2]); \
    } while(0)

    // byte swap
    for (int i = 0; i < count; ++i)
    {
        SV3(xyz); SF(st[0]); SF(st[1]);
        SF(lightmap[0]); SF(lightmap[1]);
        SV3(normal);
    }
#undef SF
#undef SV3

    bsp->verts = out;
    bsp->numVerts = count;

    printf("numVerts = %d\n", count);
}

static void _LoadIndices(Q3BSP* bsp, char* buffer, lump_t* lump)
{
    int32_t* in = (int32_t*)(buffer + lump->fileofs);
    if (lump->filelen % sizeof(uint32_t))
    {
        fprintf(stderr, "Q3BSP_Load: bad drawIndices lump size!\n");
        return;
    }
    int count = lump->filelen / sizeof(int32_t);
    int32_t* out = new int32_t[count];
    memcpy(out, in, sizeof(int32_t) * count);

    // byte swap
    for (int i = 0; i < count; ++i)
        out[i] = LittleLong(out[i]);

    bsp->indices = out;
    bsp->numIndices = count;

    printf("numIndices = %d\n", count);
}

Q3BSP* Q3BSP_Load(const char *name)
{
    char* buffer;
    int bufferSize = _LoadFileToMemory(name, &buffer);
    if (bufferSize <= 0)
    {
        fprintf(stderr, "Could not load file \"%s\"\n", name);
        return 0;
    }

    dheader_t* header = (dheader_t*)buffer;

    // check ident
    int32_t ident = LittleLong(header->ident);
    if (ident != BSP_IDENT)
    {
        fprintf(stderr, "Q3BSP_Load: \"%s\" is not a valid Quake3 BSP file\n", name);
        return 0;
    }

    // check version
    int32_t i = LittleLong(header->version);
    if (i != BSP_VERSION)
    {
        fprintf(stderr, "Q3BSP_Load: \"%s\" has wrong version number (%i should be %i)\n", name, i, BSP_VERSION);
        return 0;
    }

    // swap the lumps
    for (i = 0; i < (int32_t)sizeof(dheader_t)/4; i++)
    {
        ((int32_t *)header)[i] = LittleLong ( ((int32_t *)header)[i]);
    }

    // alloc the bsp
    Q3BSP* bsp = new Q3BSP();
    memset(bsp, 0, sizeof(Q3BSP));

    _LoadShaders(bsp, buffer, &header->lumps[LUMP_SHADERS]);
    _LoadLightmaps(bsp, buffer, &header->lumps[LUMP_LIGHTMAPS]);
    _LoadSurfaces(bsp, buffer, &header->lumps[LUMP_SURFACES]);
    _LoadVertices(bsp, buffer, &header->lumps[LUMP_DRAWVERTS]);
    _LoadIndices(bsp, buffer, &header->lumps[LUMP_DRAWINDICES]);

/*
    // load into heap
    R_LoadShaders( &header->lumps[LUMP_SHADERS] );
    R_LoadLightmaps( &header->lumps[LUMP_LIGHTMAPS] );
    R_LoadPlanes (&header->lumps[LUMP_PLANES]);
    R_LoadFogs( &header->lumps[LUMP_FOGS], &header->lumps[LUMP_BRUSHES], &header->lumps[LUMP_BRUSHSIDES] );
    R_LoadSurfaces( &header->lumps[LUMP_SURFACES], &header->lumps[LUMP_DRAWVERTS], &header->lumps[LUMP_DRAWINDEXES] );
    R_LoadMarksurfaces (&header->lumps[LUMP_LEAFSURFACES]);
    R_LoadNodesAndLeafs (&header->lumps[LUMP_NODES], &header->lumps[LUMP_LEAFS]);
    R_LoadSubmodels (&header->lumps[LUMP_MODELS]);
    R_LoadVisibility( &header->lumps[LUMP_VISIBILITY] );
    R_LoadEntities( &header->lumps[LUMP_ENTITIES] );
    R_LoadLightGrid( &header->lumps[LUMP_LIGHTGRID] );

    s_worldData.dataSize = (byte *)ri.Hunk_Alloc(0, h_low) - startMarker;

    // only set tr.world now that we know the entire level has loaded properly
    tr.world = &s_worldData;

    ri.FS_FreeFile( buffer.v );
*/

    fprintf(stderr, "Q3BSP_Load: success!\n");
    delete [] buffer;
    return bsp;
}

void Q3BSP_Free(Q3BSP* bsp)
{
    delete [] bsp->shaders;
    bsp->numShaders = 0;

    for (int i = 0; i < bsp->numLightmaps; ++i)
        delete [] bsp->lightmaps[i].data;
    delete [] bsp->lightmaps;
    bsp->numLightmaps = 0;

    delete [] bsp->surfaces;
    bsp->numSurfaces = 0;

    delete [] bsp->verts;
    bsp->numVerts = 0;

    delete [] bsp->indices;
    bsp->numIndices = 0;
}
