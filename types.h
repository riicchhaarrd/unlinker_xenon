#pragma once
#include <stdint.h>
#include <format>

template<typename T>
using Ptr32 = int32_t;
using ConstStringPtr32 = int32_t;

#ifndef COUNT_OF
#define COUNT_OF(x) (sizeof(x) / sizeof((x)[0]))
#endif // !COUNT_OF

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

struct vec3
{
	float x, y, z;

	friend std::ostream& operator<<(std::ostream& os, const vec3& v) {
		os << std::format("({}, {}, {})", v.x, v.y, v.z);
		return os;
	}
};


static const char* kAssetTypeStrings[] = {
	"kxmodelpieces",
	"kphyspreset",
	"kxanim",
	"kxmodel",
	"kmaterial",
	"ktechset",
	"kimage",
	"ksound",
	"ksndcurve",
	"kloaded_sound",
	"kcol_map_sp",
	"kcol_map_mp",
	"kcom_map",
	"kgame_map_sp",
	"kgame_map_mp",
	"kmap_ents",
	"kgfx_map",
	"klightdef",
	"kui_map",
	"kfont",
	"kmenufile",
	"kmenu",
	"klocalize",
	"kweapon",
	"ksnddriverglobals",
	"kfx",
	"kimpactfx",
	"kaitype",
	"kmptype",
	"kcharacter",
	"kxmodelalias",
	"krawfile",
	"kstringtable",
	"kmax",
	NULL
};

enum XAssetType
{
	kxmodelpieces,
	kphyspreset,
	kxanim,
	kxmodel,
	kmaterial,
	ktechset,
	kimage,
	ksound,
	ksndcurve,
	kloaded_sound,
	kcol_map_sp,
	kcol_map_mp,
	kcom_map,
	kgame_map_sp,
	kgame_map_mp,
	kmap_ents,
	kgfx_map,
	klightdef,
	kui_map,
	kfont,
	kmenufile,
	kmenu,
	klocalize,
	kweapon,
	ksnddriverglobals,
	kfx,
	kimpactfx,
	kaitype,
	kmptype,
	kcharacter,
	kxmodelalias,
	krawfile,
	kstringtable,
	kmax,
};