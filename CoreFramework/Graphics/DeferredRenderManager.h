#ifndef __DEFERREDRENDERMANAGER_H__
#define __DEFERREDRENDERMANAGER_H__

namespace GODZ
{
	// Forward Declarations
	class	IDriver;
	class	RenderDeviceObject;
	struct	SceneInfo;
	class	ViewportInfo;

	enum RenderTargetType
	{
		RT_MATERIAL,
		RT_NORMAL,
		RT_DEPTH,
		RT_DIFFUSE,
		RT_SPECULAR,
		/*
		RT_SSAO,
		RT_BLUR_X,
		RT_BLUR_Y,
		*/

		RT_COUNT,			// Number of render target
		RT_INVALID,			// Invalid render target
	};

	class DeferredRenderManager
	{
	public:
		virtual void Render(ViewportInfo &viewInfo, SceneInfo &sceneInfo, IDriver* Driver, bool isEditor) = 0;
		virtual RenderDeviceObject * GetTextureObject(RenderTargetType rtType) = 0;
	};

} // namespace GODZ

#endif // #ifndef __DEFERREDRENDERMANAGER_H__
