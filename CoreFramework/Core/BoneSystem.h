/* ===========================================================
	Skeletal Animation System

	This file is setup so that the skeletal animation 
	exporters can easily include it.

	Created Apr 4, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_BONESYSTEM_H_)
#define _BONESYSTEM_H_

#include <vector>

namespace GODZ
{
	static const unsigned long SCENE_EXPORT_MAGIC=0xFABEF;
	static const unsigned long SCENE_EXPORT_VERSION=4;

	struct MeshInfo
	{
		int iNumVertices;
		fpos_t vertsOffset;			//start vertices def
		int iNumFaces;				//faces - contains normals, texture faces, and face normal
		fpos_t facesOffset;
		int iNumTVerts;				//# of texture verts
		fpos_t tvertsOffset;
		int iNumNormals;			//optimized vertex normals
		fpos_t normOffset;
		size_t iNumBones;			//# of bones defined for this skeletal system
		fpos_t boneOffset;			//offset where animation data starts		
		fpos_t nameOffset;			//name offset
		fpos_t skinVertOffset;		//offset where skin data starts
		int iNumSkinVerts;			//skinned vertices		
		fpos_t initialSkinOffset;	//initial skin matrix offset
		fpos_t skeletonOffset;		//offset where skeletal def starts
	};

	//file header
	struct GodzExporterHeader
	{
		unsigned long uMagic;		//Magic Number
		unsigned long uVersion;		//Version Number 
		unsigned int numObjects;	//number of meshes
		int iSceneTicks;			//scene ticks per frame
		int framePerSample;			//frames per sec
		int iNumMaterials;			//number of materials
		fpos_t materialOffset;
	};

	//old GSA file format....
	struct Deprecated_GodzExporterHeader
	{
		unsigned long uMagic;		//Magic Number
		unsigned long uVersion;		//Version Number 
		int iSceneTicks;			//scene ticks per frame
		int iNumVertices;
		fpos_t vertsOffset;			//start vertices def
		int iNumFaces;				//faces - contains normals, texture faces, and face normal
		fpos_t facesOffset;
		int iNumTVerts;				//# of texture verts
		fpos_t tvertsOffset;
		int iNumNormals;			//optimized vertex normals
		fpos_t normOffset;
		int iNumMaterials;			//number of materials
		fpos_t materialOffset;
		size_t iNumBones;			//# of bones defined for this skeletal system
		fpos_t boneOffset;			//offset where animation data starts		
		fpos_t nameOffset;			//name offset
		fpos_t skinVertOffset;		//offset where skin data starts
		int iNumSkinVerts;			//skinned vertices
		int framePerSample;			//frames per sec
		fpos_t initialSkinOffset;	//initial skin matrix offset
		fpos_t skeletonOffset;		//offset where skeletal def starts
	};

	//Identifies the type of animation that is exported
	enum EExportAnimKey
	{
		EA_None=0,
		EA_BezierKeys,
		EA_FullSampleKeys
	};

	class ShapeType
	{
	public:
		enum
		{
			ShapeType_Sphere,
			ShapeType_Box,
			ShapeType_Mesh
		};
	};

}

#endif