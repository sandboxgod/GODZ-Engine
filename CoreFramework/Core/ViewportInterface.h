/* ===================================================================
	Class:	ViewportInterface

	Represents a renderable surface area (which is drawn to via the
	HWND). ViewportInterfaces are managed by the driver and are 
	created for each active window. For instance, if the mesh browser
	is open, then the render device should contain two active
	viewport interfaces- one for the main screen and a second one
	to represent the mesh browser graphics surface.

	Created March 7, 2004 by Richard Osborne
	Copyright (c) 2010
	==================================================================
*/

#pragma once

#include "Godz.h"
#include <CoreFramework/Math/WMatrix.h>
#include <CoreFramework/Math/Rect2.h>
#include "GenericObject.h"
#include "Windows.h"


namespace GODZ
{
	//forward declar
	class LevelNode;
	class WCamera;


	class GODZ_API ViewportInterface
	{
	public:
		ViewportInterface();	
		virtual ~ViewportInterface();

		//destroys this viewport
		virtual void			Destroy();

		//Returns the active camera 
		ObjectID				GetActiveCamera();

		virtual HWND			GetHWND()=0;

		int						GetWidth();
		int						GetHeight();

		//Information used to build the projection matrix
		const ProjectionMatrixInfo& GetProjectionMatrixInfo() { return m_projMat; }

		//Computes the projection matrix
		virtual void			GetProjectionMatrix(WMatrix& mat) {};

		//Presents the back buffer to the viewer
		virtual void			Present()=0;

		void					SetActiveCamera(ObjectID pCam);

		virtual void			SetBackgroundColor(UInt8 r, UInt8 g, UInt8 b) = 0;

		void					SetHWND(HWND hWnd);
		void					SetNearFarPlane(float near_plane, float far_plane);

		//set min/max of the clip volume
		virtual void			SetClipVolume(float minZ, float maxZ) {};
		virtual void			SetViewport(ulong x, ulong y, ulong width, ulong height, float minz, float miny) {};

		const Rect2&			GetRect() { return m_rect; }

	protected:
		ObjectID				m_pActiveCamera;		//handle to the scene's camera (stores view matrix)
		ProjectionMatrixInfo	m_projMat;
		HWND					m_hWnd;					//target surface
		Rect2					m_rect;
	};
}
