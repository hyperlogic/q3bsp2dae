#ifndef _Q3BSP_H_
#define _Q3BSP_H_

#include <stdint.h>

#if defined(Q_LITTLE_ENDIAN) && defined(Q_BIG_ENDIAN)
#error "Endianness defined as both big and little"
#elif defined(Q_BIG_ENDIAN)
#define LittleShort(x) ShortSwap(x)
#define LittleLong(x) LongSwap(x)
#define LittleFloat(x) FloatSwap(&x)
#define BigShort
#define BigLong
#define BigFloat
#elif defined(Q_LITTLE_ENDIAN)
#define LittleShort
#define LittleLong
#define LittleFloat
#define BigShort(x) ShortSwap(x)
#define BigLong(x) LongSwap(x)
#define BigFloat(x) FloatSwap(&x)
#else
#error "Endianness not defined"
#endif

inline int16_t ShortSwap(int16_t v)
{
    uint8_t b[2];
    b[0] = v & 255;
    b[1] = (v >> 8) & 255;
    return (b[0] << 8) + b[1];
}

inline int32_t LongSwap(int32_t v)
{
    uint8_t b[4];
    b[0] = v & 255;
    b[1] = (v >> 8) & 255;
    b[2] = (v >> 16) & 255;
    b[3] = (v >> 24) & 255;
    return ((int32_t)b[0] << 24) + ((int32_t)b[1] << 16) + ((int32_t)b[2] << 8) + b[3];
}

typedef union {
	float f;
	int i;
	unsigned int ui;
} floatint_t;

inline float FloatSwap (const float *f)
{
	floatint_t out;
	out.f = *f;
	out.ui = LongSwap(out.ui);
	return out.f;
}

#define BSP_VERSION 46
#define BSP_IDENT 0x50534249  // 'IBSP'

typedef struct {
	int32_t fileofs, filelen;
} lump_t;

#define	LUMP_ENTITIES		0
#define	LUMP_SHADERS		1
#define	LUMP_PLANES			2
#define	LUMP_NODES			3
#define	LUMP_LEAFS			4
#define	LUMP_LEAFSURFACES	5
#define	LUMP_LEAFBRUSHES	6
#define	LUMP_MODELS			7
#define	LUMP_BRUSHES		8
#define	LUMP_BRUSHSIDES		9
#define	LUMP_DRAWVERTS		10
#define	LUMP_DRAWINDICES    11
#define	LUMP_FOGS			12
#define	LUMP_SURFACES		13
#define	LUMP_LIGHTMAPS		14
#define	LUMP_LIGHTGRID		15
#define	LUMP_VISIBILITY		16
#define	HEADER_LUMPS		17

typedef struct {
	int32_t ident;
	int32_t version;

	lump_t lumps[HEADER_LUMPS];
} dheader_t;

#define	MAX_QPATH 64

typedef struct {
	char shader[MAX_QPATH];
	int surfaceFlags;
	int contentFlags;
} dshader_t;

typedef struct {
    unsigned char* data;  // rbg
    int width;
    int height;
} dlightmap_t;

typedef float vec_t;
typedef vec_t vec3_t[3];

typedef enum {
	MST_BAD,
	MST_PLANAR,
	MST_PATCH,
	MST_TRIANGLE_SOUP,
	MST_FLARE
} mapSurfaceType_t;

typedef struct {
	int32_t shaderNum;
	int32_t fogNum;
	int32_t surfaceType;

	int32_t firstVert;
	int32_t numVerts;

	int32_t firstIndex;
	int32_t numIndexes;

	int32_t lightmapNum;
	int32_t lightmapX, lightmapY;
	int32_t lightmapWidth, lightmapHeight;

	vec3_t lightmapOrigin;
	vec3_t lightmapVecs[3];	// for patches, [0] and [1] are lodbounds

	int32_t patchWidth;
	int32_t patchHeight;
} dsurface_t;

typedef struct {
	vec3_t xyz;
	float st[2];
	float lightmap[2];
	vec3_t normal;
	char color[4];
} drawVert_t;

struct Q3BSP {
    int32_t numShaders;
    dshader_t* shaders;
    int32_t numLightmaps;
    dlightmap_t* lightmaps;
    int32_t numSurfaces;
    dsurface_t* surfaces;
    int32_t numVerts;
    drawVert_t* verts;
    int32_t numIndices;
    int32_t* indices;
};

#define LIGHTMAP_SIZE 128

Q3BSP* Q3BSP_Load(const char* name);
void Q3BSP_Free(Q3BSP* bsp);

#endif
