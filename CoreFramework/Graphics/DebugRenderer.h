#ifndef _DEBUGRENDERER_H_
#define _DEBUGRENDERER_H_

#include <CoreFramework/Core/RenderList.h>

namespace GODZ
{
	class IDriver;

	class DebugRenderer
	{
	public:
		static void Render(ViewportInfo& viewInfo, SceneInfo& sceneInfo, IDriver* Driver);
	};
}

#endif // #ifndef _DEBUGRENDERER_H_
