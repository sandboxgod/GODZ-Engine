/* ===========================================================
	SceneInfo

	"Plans fail for lack of counsel, but with many advisers
	they succeed." - Proverbs 15:22

	Created June 14, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__SCENEINFO_H__)
#define __SCENEINFO_H__

#include "Godz.h"
#include <CoreFramework/Math/WMatrix.h>
#include <CoreFramework/Math/WBox.h>
#include "RenderData.h"

namespace GODZ
{
	class ViewportInterface;
	class ShadowMapManager;
	class HDRManager;
	class IDriver;
	class RenderData;

	static const int NUM_FRUSTUM_POINTS = 8;
	static const int	MAX_NUM_SPLITS=5;						//you can have up to like 10 possible split planes

	//Information about the current scene
	struct GODZ_API SceneInfo
	{
		ViewportInterface *		m_pViewport;
		size_t					m_nVertices;				//total amount of vertices rendered this frame
		size_t					m_nModelsDrawn;				//total amount of models rendered this frame	
		CameraInfo*				m_camera;					//stores view matrix & scene node we are rendering
		RenderData*	m_renderData;							//stuff we wish to render this pass
		RenderableGeometryInfo*	m_geomInfo;					//information about the current object we're rendering
		IDriver*			m_pDriver;

		//--------------------------- Shadow Mapping variables - not valid if shadows not on
		bool					m_hasShadows;
		bool					m_bUnlit_Shadow_Pass;				//currently rendering an unlit shadow pass 
		WBox					m_viewBox;							//(shadows on) box containing all the objects that are currently within the view frustum (if shadowmapping is on)
		Vector3					m_viewBoxPoints[NUM_FRUSTUM_POINTS];					//(only computed if shadows enabled) AABB points (m_viewBox points)
		float					m_fCameraFar;						//far plane for the camera if shadows on
		Vector3					m_frustumPoints[NUM_FRUSTUM_POINTS];

		SceneLight*				m_light;							//id of the light we're using this pass
		WMatrix					m_lightView;						//light view matrix
		WBox					m_lightViewBox;
		
		float					m_pSplitDistances[MAX_NUM_SPLITS+1];	// + 1 extra slot to store far_plane

		SceneInfo()
			: m_lightView(1)
			, m_hasShadows(false)
			, m_pViewport(NULL)
			, m_nVertices(0)
			, m_nModelsDrawn(0)
			, m_camera(NULL)
			, m_renderData(NULL)
			, m_geomInfo(NULL)
			, m_pDriver(NULL)
			, m_bUnlit_Shadow_Pass(false)
			, m_fCameraFar(0)
			, m_light(NULL)
		{
		}
	};
}

#endif