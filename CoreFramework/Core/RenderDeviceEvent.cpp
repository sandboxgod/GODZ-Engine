/* ===========================================================
RenderDeviceEvent

Copyright (c) 2008, Richard Osborne
=============================================================
*/

#include "RenderDeviceEvent.h"



namespace GODZ
{


RenderDeviceObjectEvent* CreateDestroyEvent(Future<GenericReference<RenderDeviceObject> >& future)
{
	RenderDeviceObjectEvent* event = new DestroyEvent();
	event->m_renderDeviceObject = future;
	return event;
}

RenderDeviceObjectEvent* CreateHDREvent(Future<GenericReference<RenderDeviceObject> >& future)
{
	RenderDeviceObjectEvent* event = new HDREvent();
	event->m_renderDeviceObject = future;
	return event;
}

VertexParamsEvent* CreateModelEvent(Future<GenericReference<RenderDeviceObject> >& future, VertexParams& params, rstring filename, bool recomputeNormals)
{
	VertexParamsEvent* event = new VertexParamsEvent();
	event->m_renderDeviceObject = future;
	event->m_params = params;
	event->m_filename = filename;
	event->m_recomputeNormals = recomputeNormals;
	return event;
}

GODZ_API RenderDeviceEvent* CreateTriMesh(Future<GenericReference<RenderDeviceObject> >& future, TriangleList& m, Color4f& color, const WMatrix& trans)
{
	TriMeshEvent* event = new TriMeshEvent();
	event->m_mesh = m;
	event->m_renderDeviceObject = future;
	event->color = color;
	event->m = trans;

	return event;
}

GODZ_API CreateCylinderEvent* CreateCylinder(Future<GenericReference<RenderDeviceObject> >& future, float radius1, float radius2, float length, unsigned int slices, unsigned int stacks)
{
	CreateCylinderEvent* event = new CreateCylinderEvent();
	event->radius1 = radius1;
	event->radius2 = radius2;
	event->length = length;
	event->slices = slices;
	event->stacks = stacks;
	event->m_renderDeviceObject = future;

	return event;
}

TextureEvent* CreateTextureEvent(Future<GenericReference<RenderDeviceObject> >& future, Future<TextureInfo>& textureInfoFuture, UInt8* imgData, long numBytes, rstring filename)
{
	TextureEvent* event = new TextureEvent();
	event->m_renderDeviceObject = future;
	event->m_numBytes = numBytes;
	event->m_filename = filename;
	event->m_texInfo = textureInfoFuture;

	for (long i=0;i<numBytes;i++)
	{
		event->m_pImageData.push_back(imgData[i]);
	}

	return event;
}


BitmapEvent* CreateHBITMAPEvent(Future<GenericReference<RenderDeviceObject> >& deviceObj, Future<GODZHANDLE>& future)
{
	BitmapEvent* event = new BitmapEvent();
	event->m_bitmap = future;
	event->m_renderDeviceObject = deviceObj;
	return event;
}

ShaderEvent* CreateShaderEvent(Future<GenericReference<RenderDeviceObject> >& future, Future<ShaderTechniqueList>& techniqueList, rstring filename)
{
	ShaderEvent* event = new ShaderEvent();
	event->m_renderDeviceObject = future;
	event->m_filename = filename;
	event->m_techniqueList = techniqueList;
	return event;
}



}