#include "q3bsp.h"
#include <stdio.h>

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

void* Q3BSP_Load(const char *name)
{
    char* buffer;
    int bufferSize = _LoadFileToMemory(name, &buffer);
    if (bufferSize <= 0)
    {
        fprintf(stderr, "Could not load file \"%s\"\n", name);
        return 0;
    }

/*
    int i;
    dheader_t *header;
    union {
        byte *b;
        void *v;
    } buffer;
    byte *startMarker;

    // load it
    ri.FS_ReadFile( name, &buffer.v );
    if ( !buffer.b ) {
        ri.Error (ERR_DROP, "RE_LoadWorldMap: %s not found", name);
    }

    Com_Memset( &s_worldData, 0, sizeof( s_worldData ) );
    Q_strncpyz( s_worldData.name, name, sizeof( s_worldData.name ) );

    Q_strncpyz( s_worldData.baseName, COM_SkipPath( s_worldData.name ), sizeof( s_worldData.name ) );
    COM_StripExtension(s_worldData.baseName, s_worldData.baseName, sizeof(s_worldData.baseName));
*/

/*
    startMarker = ri.Hunk_Alloc(0, h_low);
    c_gridVerts = 0;

    header = (dheader_t *)buffer.b;
    fileBase = (byte *)header;
*/
    dheader_t* header = (dheader_t*)buffer;
    char* fileBase = buffer;

    int32_t i = LittleLong(header->version);
    if (i != BSP_VERSION)
    {
        fprintf(stderr, "Q3BSP_Load: \"%s\" has wrong version number (%i should be %i)\n", name, i, BSP_VERSION);
        return 0;
    }

    fprintf(stderr, "Q3BSP_Load: success!\n");

/*
    i = LittleLong (header->version);
    if ( i != BSP_VERSION ) {
        ri.Error (ERR_DROP, "RE_LoadWorldMap: %s has wrong version number (%i should be %i)",
            name, i, BSP_VERSION);
    }
*/

/*
    // swap all the lumps
    for (i=0 ; i<sizeof(dheader_t)/4 ; i++) {
        ((int *)header)[i] = LittleLong ( ((int *)header)[i]);
    }

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
}

