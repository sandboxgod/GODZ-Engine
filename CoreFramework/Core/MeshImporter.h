/* ===================================================================
	Mesh Importer

	Base class for mesh importers

	Created May 1, 2004 by Richard Osborne
	Copyright (c) 2010
	==================================================================
*/

#if !defined(MESH_IMPORT_H)
#define MESH_IMPORT_H

#include "Godz.h"

namespace GODZ
{
	class MeshImporter
	{
	public:
		virtual ~MeshImporter();

	protected:
		struct Face
		{
			udword v[3];			//vertex indicies that compose a mesh face
			udword material_id;		//material used by this face

			Face() : material_id(0)
			{}
		};

		//textured vertex
		struct TVert
		{
			float u,v;

			TVert() : u(0),v(0)
			{}
		};

		//Textured faces indicates the order of textured vertices
		struct TFace
		{
			udword tv[3]; //refers to a textured vertex index
		};

		//Normal for this vertex
		struct VertexNormal
		{
			float nx,ny,nz;
			int index;		//vertex index

			VertexNormal() :nx(0),ny(0),nz(0),index(0)
			{}
		};

		struct FaceNormal
		{
			float nx,ny,nz;				//normal for this face
			VertexNormal vnorm[3];		//vertex indicies

			FaceNormal() :nx(0),ny(0),nz(0)
			{}
		};
	};
}


#endif
