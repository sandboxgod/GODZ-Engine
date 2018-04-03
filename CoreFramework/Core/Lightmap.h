/* ===========================================================

	Lightmaps
	
	http://www.alsprogrammingresource.com/lightmapping_tutorial.html

	Created Dec 13, '06 by Richard Osborne
	Copyright (c) 2003-4, Richard Osborne
	========================================================== 
*/

#if !defined(__LIGHTMAPS_H__)
#define __LIGHTMAPS_H__

#include "Godz.h"
#include "TextureResource.h"
#include "Color4f.h"
#include "VertexTypes.h"
#include <CoreFramework/Math/WPlane.h>

namespace GODZ
{
	//static light bulb placed in the world
	struct GODZ_API StaticLight
	{
		Vector3 Position;
		float Brightness;
		float Radius;
		Color4f color;
	};

	struct GODZ_API Polygon
	{
		WPlane plane;
		LightmapVertex Vertex[3];		//3 vertices compose a triangle
	};

	struct GODZ_API Lightmap
	{
		TextureResource* m_tex;
	};	

	GODZ_API void CreateLightmaps(int numpolys, Polygon* polylist, Lightmap* lightmaplist, StaticLight* staticlight, size_t numStaticLights);
}

#endif