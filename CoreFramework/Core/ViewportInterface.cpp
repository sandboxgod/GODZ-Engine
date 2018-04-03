
#include "ViewportInterface.h"
#include "WCamera.h"
#include "GodzAtomics.h"

using namespace GODZ;

ViewportInterface::ViewportInterface()
: m_pActiveCamera(INVALID_ID)
, m_hWnd(NULL)
{
	m_projMat.far_plane = FARPLANE; ///so that we know this projection is not initialized
	m_projMat.near_plane = NEARPLANE;
	m_projMat.fov = DEG2RAD(45);
}

ViewportInterface::~ViewportInterface()
{
}

void ViewportInterface::Destroy()
{
	m_pActiveCamera=INVALID_ID;
}

ObjectID ViewportInterface::GetActiveCamera()
{
	return m_pActiveCamera;
}


void ViewportInterface::SetActiveCamera(ObjectID pCam)
{
	m_pActiveCamera = pCam;
}

int	ViewportInterface::GetWidth() 
{ 
	return m_rect.right - m_rect.left; 
}

int	ViewportInterface::GetHeight() 
{ 
	return m_rect.bottom - m_rect.top; 
}

void ViewportInterface::SetHWND(HWND hWnd)
{
	this->m_hWnd=hWnd;
	tagRECT lpRect;

	BOOL hr =  GetWindowRect(hWnd,&lpRect);
	godzassert(hr == TRUE);
	int w = lpRect.right - lpRect.left;
	int h = lpRect.bottom - lpRect.top;

	m_rect.left = lpRect.left;
	m_rect.right = lpRect.right;
	m_rect.top = lpRect.top;
	m_rect.bottom = lpRect.bottom;

	m_projMat.aspectRatio = float(w) / float(h);
}

void ViewportInterface::SetNearFarPlane(float near_plane, float far_plane)
{
	m_projMat.far_plane = far_plane;
	m_projMat.near_plane = near_plane;
}
