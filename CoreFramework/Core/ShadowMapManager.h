/* ===========================================================
	Shadow Map Manager

	Created Jun 15, '06 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__SHADOWMAPMANAGER__)
#define __SHADOWMAPMANAGER__

#include "Godz.h"
#include "GenericObject.h"
#include <CoreFramework/Math/Vector2.h>

namespace GODZ
{
	class RenderDeviceObject;
	class ShaderResource;
	struct SceneInfo;

	enum ShadowMapQuality
	{
		ShadowMapQuality_Low,
		ShadowMapQuality_Med,
		ShadowMapQuality_High,
		ShadowMapQuality_Ultra
	};

	class GODZ_API ShadowMapManager
	{

	public:
		ShadowMapManager();
		virtual ~ShadowMapManager() {}

		virtual void					BeginSplit(unsigned int splitIndex, SceneInfo* sceneInfo) {}
		virtual void					Begin(unsigned int splitIndex, SceneInfo* sceneInfo);
		virtual void					End(unsigned int splitIndex);
		virtual void					TakeSnapshot() {}
		virtual float					GetShadowMapSize() = 0;

		virtual RenderDeviceObject *	GetShadowMapTextureObject(int iSplit) { return NULL; }

		static const unsigned int MAX_NUM_SHADOW_MAPS = 5;

		static const Vector2& GetAtlasSplit(int index)
		{
			godzassert(index < 4);

			static const Vector2 atlasOffsets[4] = { Vector2(0.0f, 0.0f), // split 1 - upper left
			Vector2(0.5f, 0.0f), // split 2 - upper right
			Vector2(0.0f, 0.5f), // split 3 - lower left
			Vector2(0.5f, 0.5f) }; // split 4 - lower right

			return atlasOffsets[index];
		}
	};
}

#endif