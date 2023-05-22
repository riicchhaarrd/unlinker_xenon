#include <iostream>
#include <fstream>
#include <format>
#include <print>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#pragma comment(lib, "zlib.lib")
#include <zlib.h>
#include <zconf.h>
#include <assert.h>
#include <map>
#include <unordered_map>
#include <memory>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "util.h"
#include "types.h"
#include "buffer_reader.h"

struct XFile
{
	uint32_t idk; //size?
	uint32_t uncompressed_size;
	uint32_t blockSize[3];

	void read(BufferReader& br)
	{
		idk = br.read_u32();
		uncompressed_size = br.read_u32();
		for(int i = 0; i < COUNT_OF(blockSize); ++i)
			blockSize[i] = br.read_u32();
	}
};
enum XAssetEnum
{
	parts,
	model,
	material,
	image,
	sound,
	sndCurve,
	clipMap,
	world,
	lightDef,
	font,
	menuList,
	menu,
	localize,
	weapon,
	sndDriverGlobals,
	fx,
	impactFx,
	rawfile,
	data
};
static const char *xasset_enums_strings[] = {
	"parts",
	"model",
	"material",
	"image",
	"sound",
	"sndCurve",
	"clipMap",
	"world",
	"lightDef",
	"font",
	"menuList",
	"menu",
	"localize",
	"weapon",
	"sndDriverGlobals",
	"fx",
	"impactFx",
	"rawfile",
	"data"
};

struct GfxLight
{
	u32 def;
	float position[4];
	vec3 color;
	//GfxLightInfo->GfxLightDir
	vec3 dir;
	void read(BufferReader& br)
	{
		def = br.read_u32();
		for(int i = 0; i < 4; ++i)
			position[i] = br.read_float();
		color = br.read_vec3();
		dir = br.read_vec3();
	}
};

constexpr int sizeof_GfxLight = sizeof(GfxLight);

static std::string xasset_enum_to_string(u32 e)
{
	if (e >= COUNT_OF(xasset_enums_strings))
		return "";
	return xasset_enums_strings[e];
}

struct XAssetHeader
{
	void* parts;
	void* model;
	void* material;
	void* image;
	void* sound;
	void* sndCurve;
	void* clipMap;
	void* world;
	void* lightDef;
	void* font;
	void* menuList;
	void* menu;
	void* localize;
	void* weapon;
	void* sndDriverGlobals;
	void* fx;
	void* impactFx;
	void* rawfile;
	void* data;
};

struct XAssetList
{
	uint32_t scriptStringCount;
	u32 scriptStrings;
	uint32_t assetCount;
	u32 assets;
	u32 idk;
	u32 idk2;

	std::vector<std::string> strings;

	void read(BufferReader& br)
	{
		scriptStringCount = br.read_u32();
		scriptStrings = br.read_u32();
		assetCount = br.read_u32();
		assets = br.read_u32();
		idk = br.read_u32();
		idk2 = br.read_u32();
	}

	std::string string(u32 index)
	{
		if (index - 1 >= scriptStringCount)
			return "";
		return strings[index - 1];
	}
};

struct XAsset
{
	XAssetEnum type;
	u32 data;
};

struct GfxWorldVertexData
{
	u32 vertices;
	u32 worldVb;
	void read(BufferReader& br)
	{
		vertices = br.read_u32();
		worldVb = br.read_u32();
	}
};
struct SunLightParseParams
{
	char name[64];
	float ambientScale;
	vec3 ambientColor;
	float diffuseFraction;
	float sunLight;
	vec3 sunColor;
	vec3 diffuseColor;
	u8 diffuseColorHasBeenSet;
	u8 pad[3];
	vec3 angles;
	void read(BufferReader& br)
	{
		for (int i = 0; i < 64; ++i)
			name[i] = br.read<char>();
		ambientScale = br.read_float();
		ambientColor = br.read_vec3();
		diffuseFraction = br.read_float();
		sunLight = br.read_float();
		sunColor = br.read_vec3();
		diffuseColor = br.read_vec3();
		diffuseColorHasBeenSet = br.read<char>();
		br.read<char>();
		br.read<char>();
		br.read<char>();
		angles = br.read_vec3();
	}
};

struct GfxWorld
{
	u32 name;
	u32 baseName;
	u32 nodeCount;
	u32 nodes;
	u32 surfaceCount;
	u32 surfaces;
	u32 skySurfCount;
	u32 skyStartSurfs;
	u32 skyImage;
	u8 skySamplerState;
	u8 pad[3];
	u32 vertexCount;
	GfxWorldVertexData vd;
	SunLightParseParams sunParse;
	GfxLight sunLight;
	vec3 sunColorFromBsp;
	u32 cullGroupCount;
	u32 cullGroups;
	u32 smodelCount;
	u32 smodelInsts;
	u32 cellCount;
	u32 cells;
	u32 lightmapCount;
	u32 lightmaps;
	u32 smodelLightingImage;
	vec3 smodelLightingLookupScale;
	void read(BufferReader& br)
	{
		name = br.read_u32();
		baseName = br.read_u32();
		nodeCount = br.read_u32();
		nodes = br.read_u32();
		surfaceCount = br.read_u32();
		surfaces = br.read_u32();
		skySurfCount = br.read_u32();
		skyStartSurfs = br.read_u32();
		skyImage = br.read_u32();
		skySamplerState = br.read<u8>();
		br.read<u8>();
		br.read<u8>();
		br.read<u8>();
		vertexCount = br.read_u32();
		vd.read(br);
		sunParse.read(br);
		sunLight.read(br);
		sunColorFromBsp = br.read_vec3();
		cullGroupCount = br.read_u32();
		cullGroups = br.read_u32();
		smodelCount = br.read_u32();
		smodelInsts = br.read_u32();
		cellCount = br.read_u32();
		cells = br.read_u32();
		lightmapCount = br.read_u32();
		lightmaps = br.read_u32();
		smodelLightingImage = br.read_u32();
		smodelLightingLookupScale = br.read_vec3();
	}
};

enum class GfxImageFormat
{
	kInvalid,
	kBitmapRGBA, //D3DFMT_A8R8G8B8
	kBitmapRGB, //D3DFMT_X8R8G8B8
	kBitmapLuminanceAlpha, //D3DFMT_A8L8
	kBitmapLuminance, //D3DFMT_L8
	kBitmapAlpha, //D3DFMT_A8
	kWaveletRGBA, //D3DFMT_A8R8G8B8
	kWaveletRGB, //D3DFMT_X8R8G8B8
	kWaveletLuminanceAlpha, //D3DFMT_A8L8
	kWaveletLuminance, //D3DFMT_L8
	kWaveletAlpha, //D3DFMT_A8
	kDXT1, //compressed dxt
	kDXT3, //compressed dxt
	kDXT5 //compressed dxt
};

struct Material
{
	u32 name;
	u32 refImageName;
	u8 pad[12];
	u32 stateBits[2];
	u16 textureCount;
	u16 constantCount;
	u32 techniqueSet;
	u32 textures;
	u32 constants;
	void read(BufferReader& br)
	{
		name = br.read_u32();
		refImageName = br.read_u32();
		br.cursor += 12;
		stateBits[0] = br.read_u32();
		stateBits[1] = br.read_u32();
		textureCount = br.read_u16();
		constantCount = br.read_u16();
		techniqueSet = br.read_u32();
		textures = br.read_u32();
		constants = br.read_u32();
	}
};

struct GfxImageLoadDef
{
	u8 format; //DXT1 = 11, DXT3 = 12, DXT = 13
	u8 flags; //NOT SURE
	u16 dimensions[3];
	u32 data;
	u32 resourceSize;

	void read(BufferReader& br)
	{
		format = br.read<u8>();
		flags = br.read<u8>();
		for(int i = 0; i < 3; ++i)
			dimensions[i] = br.read_u16();
		data = br.read_u32();
		resourceSize = br.read_u32();
	}
};

struct GfxImageFileHeader
{
	u8 tag[3];
	u8 version;
	u8 format;
	u8 flags;
	u16 dimensions[3];
	u32 fileSizeForPicmip[4];
};

constexpr int sizeof_GfxImageFileHeader = sizeof(GfxImageFileHeader);

struct clipMap_t
{
	u32 name;
	u32 numStaticModels;
	u32 staticModelList;
	u32 numMaterials;
	u32 materials;
	u32 numBrushSides;
	u32 brushsides;
	u32 numNodes;
	u32 nodes;
	u32 numLeafs;
	u32 leafs;
	u32 leafbrushNodesCount;
	u32 leafbrushNodes;
	u32 numLeafBrushes;
	u32 leafbrushes;
	u32 numLeafSurfaces;
	u32 leafsurfaces;
	u32 vertCount;
	u32 verts;
	u32 edgeCount;
	u32 edges;
	u32 triCount;
	u32 tris;
	u32 borderCount;
	u32 borders;
	u32 partitionCount;
	u32 partitions;
	u32 aabbTreeCount;
	u32 aabbTrees;
	u32 numSubModels;
	u32 cmodels;
	u16 numBrushes;
	char pad0[2];
	u32 brushes;
	u32 numClusters;
	u32 clusterBytes;
	u32 visibility;
	u32 vised;
	u32 numEntityChars;
	u32 entityString;
	u32 box_brush;
	char box_model[72];
	u32 pathNodeCount;
	u32 pathNodes;
	u32 chainNodeCount;
	u32 chainNodes;
	u32 chainNodesRev;
	u32 visBytes;
	u32 pathVis;
	u32 nodeTreeCount;
	u32 nodeTree;
	u32 checksum;

	void read(BufferReader& br)
	{
		name = br.read_u32();
		numStaticModels = br.read_u32();
		staticModelList = br.read_u32();
		numMaterials = br.read_u32();
		materials = br.read_u32();
		numBrushSides = br.read_u32();
		brushsides = br.read_u32();
		numNodes = br.read_u32();
		nodes = br.read_u32();
		numLeafs = br.read_u32();
		leafs = br.read_u32();
		leafbrushNodesCount = br.read_u32();
		leafbrushNodes = br.read_u32();
		numLeafBrushes = br.read_u32();
		leafbrushes = br.read_u32();
		numLeafSurfaces = br.read_u32();
		leafsurfaces = br.read_u32();
		vertCount = br.read_u32();
		verts = br.read_u32();
		edgeCount = br.read_u32();
		edges = br.read_u32();
		triCount = br.read_u32();
		tris = br.read_u32();
		borderCount = br.read_u32();
		borders = br.read_u32();
		partitionCount = br.read_u32();
		partitions = br.read_u32();
		aabbTreeCount = br.read_u32();
		aabbTrees = br.read_u32();
		numSubModels = br.read_u32();
		cmodels = br.read_u32();
		numBrushes = br.read_u16();
		br.read_u16();//padding
		brushes = br.read_u32();
		numClusters = br.read_u32();
		clusterBytes = br.read_u32();
		visibility = br.read_u32();
		vised = br.read_u32();
		numEntityChars = br.read_u32();
		entityString = br.read_u32();
		box_brush = br.read_u32();
		for(int i = 0; i < 72; ++i)
			box_model[i] = br.read<u8>();
		pathNodeCount = br.read_u32();
		pathNodes = br.read_u32();
		chainNodeCount = br.read_u32();
		chainNodes = br.read_u32();
		chainNodesRev = br.read_u32();
		visBytes = br.read_u32();
		pathVis = br.read_u32();
		nodeTreeCount = br.read_u32();
		nodeTree = br.read_u32();
		checksum = br.read_u32();
	}
};
//(\w+):\([0-9]+,[0-9]+\),[0-9]+,([0-9]+)
//be clipMap_t cm @ 0x31D90;
#pragma pack(push, 1)

struct XModelLodInfo
{
	float dist;
	u32 filename;
	u16 numsurfs;
	char pad0[2];
	u32 surfNames;
	u32 surfs;
	void read(BufferReader& br)
	{
		dist = br.read_float();
		filename = br.read_u32();
		numsurfs = br.read_u16();
		br.read_u16();//padding
		surfNames = br.read_u32();
		surfs = br.read_u32();
	}
};

struct XModel
{
	u32 parts;
	XModelLodInfo lodInfo[4];
	u32 collSurfs;
	u32 numCollSurfs;
	u32 contents;
	u32 boneInfo;
	vec3 mins;
	vec3 maxs;
	u16 numLods;
	u16 collLod;
	u32 xskins;
	u32 memUsage;
	u32 name;

	//not sure whether it's u8,padding,u8
	u16 flags;
	u16 bad;
	void read(BufferReader& br)
	{
		parts = br.read_u32();
		for (int i = 0; i < 4; ++i)
		{
			lodInfo[i].read(br);
		}
		collSurfs = br.read_u32();
		numCollSurfs = br.read_u32();
		contents = br.read_u32();
		boneInfo = br.read_u32();
		mins = br.read_vec3();
		maxs = br.read_vec3();
		numLods = br.read_u16();
		collLod = br.read_u16();
		xskins = br.read_u32();
		memUsage = br.read_u32();
		name = br.read_u32();
		flags = br.read<u16>();
		bad = br.read<u16>();
	}
};
struct DSkel
{
	//not sure if it's u8 or int, lot of \xff values so i don't think it's a float
	char animPartBits[16];
	char controlPartBits[16];
	char skelPartBits[16];
	char mat[32];
	void read(BufferReader& br)
	{
		for (int i = 0; i < 16; ++i)
			animPartBits[i] = br.read<u8>();
		for (int i = 0; i < 16; ++i)
			controlPartBits[i] = br.read<u8>();
		for (int i = 0; i < 16; ++i)
			skelPartBits[i] = br.read<u8>();
		for (int i = 0; i < 32; ++i)
			mat[i] = br.read<u8>();
	}
};
struct XModelParts_s
{
	u16 numBones;
	u16 numRootBones;
	u32 hierarchy;
	u32 quats;
	u32 trans;
	u32 partClassification;
	DSkel skel;
	void read(BufferReader& br)
	{
		numBones = br.read_u16();
		numRootBones = br.read_u16();
		hierarchy = br.read_u32();
		quats = br.read_u32();
		trans = br.read_u32();
		partClassification = br.read_u32();
		skel.read(br);
	}
};

struct cStaticModelWritable
{
	u16 nextModelInWorldSector;
};

struct cStaticModel_s
{
	cStaticModelWritable writable;
	char pad0[2];
	u32 xmodel;
	vec3 origin;
	vec3 invAxis[3];
	vec3 absmin;
	vec3 absmax;
	void read(BufferReader& br)
	{
		writable.nextModelInWorldSector = br.read_u16();
		br.read_u16();//padding
		xmodel = br.read_u32();
		origin = br.read_vec3();
		invAxis[0] = br.read_vec3();
		invAxis[1] = br.read_vec3();
		invAxis[2] = br.read_vec3();
		absmin = br.read_vec3();
		absmax = br.read_vec3();
	}
	void display()
	{
		println("writable={}", writable.nextModelInWorldSector);
		println("xmodel={}", xmodel);
		println("origin={}", origin);
		println("invAxis[0]={}", invAxis[0]);
		println("invAxis[1]={}", invAxis[1]);
		println("invAxis[2]={}", invAxis[2]);
		println("absmin={}", absmin);
		println("absmax={}", absmax);
	}
};
#pragma pack(pop)
constexpr int sizeof_cStaticModel_s = sizeof(cStaticModel_s);

int main(int argc, char **argv)
{
	const char* path = "F:\\xbox\\extracted\\2\\mp_harbor.ff";
#if 0
	if (argc < 2)
		return 0;
	const char* path = argv[1];
#endif
	auto v = read_file(path);
	BufferReader br_uncomp(v.data(), v.size());
	XFile xf;
	xf.read(br_uncomp);
	printf("XFile: uncompressed_size=%d, idk=%d (0x%x)\n", xf.uncompressed_size, xf.idk, xf.idk);
	u32 total = 0;
	for(int i = 0; i < COUNT_OF(xf.blockSize); ++i)
	{
		printf("\tblockSize[%d] = %d (0x%x)\n", i, xf.blockSize[i], xf.blockSize[i]);
		total += xf.blockSize[i];
	}
	printf("total: %u\n", total);
	std::vector<unsigned char> uncomp;
	size_t offsz = 0x14;
	test_large_inflate(v.data() + offsz, v.size() - offsz, uncomp);
	printf("compressed %d bytes\n", v.size());
	printf("uncompressed %d bytes\n", uncomp.size());

	//write_file("F:\\export\\dump3.bin", uncomp);

	XAssetList list;
	BufferReader br(uncomp.data(), uncomp.size());
	list.read(br);

	printf("scriptStringCount: %d\n", list.scriptStringCount);
	printf("scriptStrings: %d\n", list.scriptStrings);
	printf("assetCount: %d\n", list.assetCount);
	printf("assets: %d\n", list.assets);
	printf("idk: %d\n", list.idk);
	printf("idk2: %d\n", list.idk2);
	printf("offset: %d (0x%x)\n", br.cursor, br.cursor);
//	getchar();

#if 1
	for (int i = 0; i < list.scriptStringCount; ++i)
	{
		br.read_u32();//-1
	}
	printf("offset: %d (0x%x)\n", br.cursor, br.cursor);
	//	getchar();
#endif
	for (int i = 0; i < list.scriptStringCount; ++i)
	{
		std::string str = br.read_string();
		list.strings.push_back(str);
		//printf("\t%d: %s\n", i, str.c_str());
	}
	//printf("%s, %s\n", strings[53 - 1].c_str(), strings[4326 - 1].c_str());
	printf("%d strings\n", list.strings.size());
	printf("offset: %d (0x%x)\n", br.cursor, br.cursor);
	//	getchar();

#define MEMORY_TYPE_MASK 0xE0000000
#define OBJECT_OFFSET_MASK 0x1FFFFFFF

#define GET_MEMORY_TYPE(value) ((value & MEMORY_TYPE_MASK) >> 29)
#define GET_OBJECT_OFFSET(value) (value & OBJECT_OFFSET_MASK)

	//read all the pointers for the assets?
	for (size_t i = 0; i < list.assetCount; ++i)
	{
		//usually -1, but might be some offset or relative address
		u32 ptr = br.read_u32();
		uint8_t memoryType = GET_MEMORY_TYPE(ptr);
		uint32_t objectOffset = GET_OBJECT_OFFSET(ptr);
		printf("asset %d: ptr = %x, memoryType = %x, objectOffset = %x\n", i, ptr, memoryType, objectOffset);
		//printf("asset %d: ptr = %x\n", i, ptr);
	}
	printf("offset: %d (0x%x)\n", br.cursor, br.cursor);
	//	getchar();

#if 1
	//for(int i = 0; i < 225; i++)
	for(int i = 0;;i++) //can't find any length, hackish fix, scan till we encounter double \0\0, this may be wrong and not work at all
	{
		int cursor = br.cursor;
		int ch = br.read<u8>();
		br.cursor = cursor;
		if (ch == 0)
			break;

		std::string str = br.read_string();
		//printf("%d: str=%s, offset=%x\n", i, str.c_str(), br.cursor);
	}
#endif
//	br.cursor = 0x29f82;
	std::vector<XAsset> assets;
	for (size_t i = 0; i < list.idk; ++i) //2455
	{
		u32 asset_type = br.read_u32();
		u32 header = br.read_u32();
		printf("%d: type: %d '%s', header: %d\n", i, asset_type, xasset_enum_to_string(asset_type).c_str(), header);
		assets.push_back(XAsset{ .type = (XAssetEnum)asset_type, .data = header });
	}
	printf("offset: %d (0x%x)\n", br.cursor, br.cursor);
	bool err = false;
	for (int i = 0; i < list.idk; ++i)
	{
		auto& asset = assets[i];
		printf("%s -> offset: %d (0x%x)\n", xasset_enum_to_string(asset.type).c_str(), br.cursor, br.cursor);
		switch (asset.type)
		{
		case XAssetEnum::world:
		{
			clipMap_t cm;
			cm.read(br);
			printf("after %s -> offset: %d (0x%x)\n", xasset_enum_to_string(asset.type).c_str(), br.cursor, br.cursor);

//			FILE* obj = 0;
//			fopen_s(&obj, "F:\\export\\xbox.obj", "w");
			int num = 0;
			for (int k = 0; k < cm.numStaticModels; ++k)
			{
				cStaticModel_s sm;
				sm.read(br);
				if (sm.xmodel == UINT_MAX)
				{
					++num;
				}
			}
			printf("num=%d\n", num);
			//read all? or count all -1/ff pointers and read that many?
			for (int k = 0; k < cm.numStaticModels; ++k)
			{
				XModel xm;
				xm.read(br);
				for (int g = 0; g < 4; ++g) {
					printf("Xmodel: %f %s\n", xm.lodInfo[g].dist, list.string(xm.lodInfo[g].filename).c_str());
				}
				getchar();
			}
			printf("after %s -> offset: %d (0x%x)\n", xasset_enum_to_string(asset.type).c_str(), br.cursor, br.cursor);
#if 0
			br.cursor = 0x58F3B98;
			GfxWorld w;
			w.read(br);
			println("\tname: {}", list.string(w.name));
			println("\tbaseName: {}", list.string(w.baseName));
#define PRINT(x) \
			println("\t" #x ": {}", x);

			PRINT(w.nodeCount)
			PRINT(w.nodes)
			PRINT(w.skyImage)
			PRINT(w.skySamplerState)
			PRINT(w.skyStartSurfs)
			PRINT(w.surfaceCount)
			PRINT(w.surfaces)
			PRINT(w.vertexCount)
			PRINT(w.vd.vertices)
			PRINT(w.vd.worldVb)
			PRINT(w.sunParse.ambientColor)
			PRINT(w.sunParse.ambientScale)
			PRINT(w.sunParse.angles)
			PRINT(w.sunParse.diffuseColor)
			PRINT(w.sunParse.diffuseColorHasBeenSet)
			PRINT(w.sunParse.diffuseFraction)
			PRINT(w.sunParse.name)
			PRINT(w.sunParse.sunColor)
			PRINT(w.sunParse.sunLight)
			PRINT(w.sunColorFromBsp)
			PRINT(w.cullGroupCount)
			PRINT(w.cullGroups)
			PRINT(w.smodelCount)
			PRINT(w.smodelInsts)
			PRINT(w.cellCount)
			PRINT(w.cells)
			PRINT(w.lightmapCount)
			PRINT(w.lightmaps)
			PRINT(w.smodelLightingImage)
			PRINT(w.smodelLightingLookupScale)
#endif
		} break;
		case XAssetEnum::material:
		{
			Material m;
			m.read(br);
			println("\tname: {}", list.string(m.name));
			println("\trefImageName: {}", list.string(m.refImageName));
			println("\tconstantCount: {}", m.constantCount);
			println("\tstateBits[0]: {}", m.stateBits[0]);
			println("\tstateBits[1]: {}", m.stateBits[1]);
			println("\ttextureCount: {}", m.textureCount);
			println("\ttechniqueSet: {}", std::format("{:x}", m.techniqueSet));
			//TODO: allocate/read pointers correctly according to memory type/offset instead of just -1
			auto read_technique_set = [&]()
			{
#pragma pack(push, 1)
				struct MaterialPassDx7
				{
					u32 stateMap;
					u8 gridLighting;
					u8 projectToInfinity;
					u8 ambientLighting;
					u8 objectiveGlow;
					u8 fogToBlack;
					u16 genTexCoords;
					u8 padding;
					u8 samplers[16];
					u8 colorStageBits[32];
					u8 alphaStageBits[32];
					void read(BufferReader& br)
					{
						stateMap = br.read_u32();
						gridLighting = br.read<u8>();
						projectToInfinity = br.read<u8>();
						ambientLighting = br.read<u8>();
						objectiveGlow = br.read<u8>();
						fogToBlack = br.read<u8>();
						genTexCoords = br.read_u16();
						padding = br.read<u8>();
						for (int i = 0; i < 16; ++i)
							samplers[i] = br.read<u8>();
						for (int i = 0; i < 32; ++i)
							colorStageBits[i] = br.read<u8>();
						for (int i = 0; i < 32; ++i)
							alphaStageBits[i] = br.read<u8>();
					}
				};
				struct MaterialArgumentCodeConst
				{
					u16 index;
					u8 firstRow;
					u8 rowCount;
				};
				struct MaterialStreamRouting
				{
					u8 source;
					u8 dest;
				};
				union MaterialArgumentDef
				{
					u32 literalConst;
					MaterialArgumentCodeConst codeConst;
					u32 codeSampler;
					u32 name;
				};
				struct MaterialShaderArgument
				{
					u16 type;
					u16 dest;
					MaterialArgumentDef u;
				};
				union MaterialShaderPtr
				{
					u32 vs;
					u32 ps;
				};
				struct MaterialShader
				{
					u32 name;
					u32 program;
					u16 programLen;
					u8 shaderType;
					u8 shaderVersion;
					MaterialShaderPtr u;
				};

				struct MaterialVertexDeclaration
				{
					u32 data;
					u32 streamCount;
					u32 decl[4];
					void read(BufferReader& br)
					{
						data = br.read_u32();
						streamCount = br.read_u32();
						decl[0] = br.read_u32();
						decl[1] = br.read_u32();
						decl[2] = br.read_u32();
						decl[3] = br.read_u32();
						printf("data=%x,streamCount=%x,decl[0]=%x,decl[1]=%x,decl[2]=%x,decl[3]=%x\n", data, streamCount, decl[0], decl[1], decl[2], decl[3]);
						getchar();
					}
				};

				struct MaterialPassDx9
				{
					u32 stateMap;
					u32 vertexDecl;
					u32 vertexShader;
					u32 pixelShader;
					u16 vertexArgCount;
					u16 pixelArgCount;
					u32 vertexArgs;
					u32 pixelArgs;

					void read(BufferReader& br)
					{
						stateMap = br.read_u32();
						vertexDecl = br.read_u32();
						vertexShader = br.read_u32();
						pixelShader = br.read_u32();
						vertexArgCount = br.read_u16();
						pixelArgCount = br.read_u16();
						vertexArgs = br.read_u32();
						pixelArgs = br.read_u32();

						printf("vertexDecl=%x\n", vertexDecl);
						printf("vertexShader=%x\n", vertexShader);
						printf("pixelShader=%x\n", pixelShader);
						printf("vertexArgCount=%x\n", vertexArgCount);
						printf("pixelArgCount=%x\n", pixelArgCount);
						printf("vertexArgs=%x\n", vertexArgs);
						printf("pixelArgs=%x\n", pixelArgs);

						{
							MaterialVertexDeclaration _;
							_.read(br);
						}
					}
				};
#pragma pack(pop)
				constexpr int sizeof_MaterialPassDx7 = sizeof(MaterialPassDx7);
				constexpr int sizeof_MaterialPassDx9 = sizeof(MaterialPassDx9);
				union MaterialPassArray
				{
					MaterialPassDx7 dx7;
					MaterialPassDx9 dx9;
					void read(BufferReader& br)
					{
						dx9.read(br);
					}
				};
				struct MaterialTechnique
				{
					u32 name;
					u16 flags;
					u16 passCount;
					MaterialPassArray passArray;
					void read(BufferReader& br)
					{
						name = br.read_u32();
						flags = br.read_u16();
						passCount = br.read_u16();
						passArray.read(br);
					}
				};
				constexpr int sizeof_MaterialTechnique = sizeof(MaterialTechnique);
				struct MaterialTechniqueSet
				{
					u32 name;
					u32 techniques[34];
					void read(BufferReader& br)
					{
						name = br.read_u32();
						for (int i = 0; i < 34; ++i)
						{
							techniques[i] = br.read_u32();
						}
					}
				};
				constexpr int sizeof_MaterialTechniqueSet = sizeof(MaterialTechniqueSet);
				MaterialTechniqueSet ts;
				ts.read(br);
				println("\ttechniqueSet: {}", list.string(ts.name));
				for (int i = 0; i < 34; ++i)
				{
					//TODO: MOVE?
					MaterialTechnique mt;
					mt.read(br);
					println("\MaterialTechnique: {}", list.string(mt.name));
					break;
				}
				printf("offset: %d (0x%x)\n", br.cursor, br.cursor);
				getchar();
			};
			read_technique_set();
		} break;
		default:
			err = true;
			printf("unhandled asset type '%s'\n", xasset_enum_to_string(asset.type).c_str());
			break;
		}
		if (err)
			break;
	}
#if 0
	br.cursor = 0xb06d7; //mp_harbor_load.ff
	GfxImageLoadDef def;
	def.read(br);

	uint8_t memoryType = GET_MEMORY_TYPE(def.data);
	uint32_t objectOffset = GET_OBJECT_OFFSET(def.data);
	printf("ImageLoadDef: ptr = %x, memoryType = %x, objectOffset = %x\n", def.data, memoryType, objectOffset);
	for(int i = 0; i < 3; ++i)
		println("dimensions[{}] = {}", i, def.dimensions[i]);
	println("format = {}", (int)def.format);
	println("resourceSize = {}", std::format("{:x}", def.resourceSize));
#endif

#if 0
//	u32 world_name_offset = br.read_u32();
//	printf("world name offset: %s\n", strings[world_name_offset].c_str());
	int i = 0;
	while (1)
	{
		u32 o = br.read_u32();
		printf("%d: %s\n", i, strings[o].c_str());
		++i;
		getchar();
	}
#endif

	getchar();
	return 0;
}
