#ifndef _RENDERERHELPER_H_
#define _RENDERERHELPER_H_

namespace GODZ
{
	// Forward Declarations
	class	IDriver;
	struct	SceneInfo;
	class	ViewportInfo;

	class RendererHelper
	{
	public:
		static void Render(ViewportInfo &viewInfo, SceneInfo &sceneInfo, IDriver* Driver, bool isEditor); 
	};

} // namespace GODZ

#endif // #ifndef _RENDERERHELPER_H_
