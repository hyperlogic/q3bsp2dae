#ifndef _Q3BSP_H_
#define _Q3BSP_H_

#include <stdint.h>

#define LITTLE_ENDIAN

#if defined(LITTLE_ENDIAN) && defined(BIG_ENDIAN)
#error "Endianness defined as both big and little"
#elif defined(BIG_ENDIAN)
#define LittleShort(x) ShortSwap(x)
#define LittleLong(x) LongSwap(x)
#define LittleFloat(x) FloatSwap(&x)
#define BigShort
#define BigLong
#define BigFloat
#elif defined(LITTLE_ENDIAN)
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

#define BSP_VERSION 46

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
#define	LUMP_DRAWINDEXES	11
#define	LUMP_FOGS			12
#define	LUMP_SURFACES		13
#define	LUMP_LIGHTMAPS		14
#define	LUMP_LIGHTGRID		15
#define	LUMP_VISIBILITY		16
#define	HEADER_LUMPS		17

typedef struct {
	int32_t ident;
	int32_t version;

	lump_t		lumps[HEADER_LUMPS];
} dheader_t;

void* Q3BSP_Load(const char* name);

#endif
