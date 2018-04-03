/* ===================================================================
	Render Device Object

	Created May 18, 2008 by Richard Osborne
	Copyright (c) 2010
	==================================================================
*/

#if !defined(__RenderDeviceObject_H__)
#define __RenderDeviceObject_H__

#include "Godz.h"
#include "GenericReferenceCounter.h"
#include <hash_map>

namespace GODZ
{	
	/*
	* This object is instantiated by the render thread and pretty much owned by
	* that thread. The main thread and other outside threads are allowed to reference
	* this object however, they cannot make any calls to it. The main reason
	* we use this Class so that the render thread can quickly cast this object
	* to the local render pipeline type (helps avoid hash table lookup costs if we used
	* hash codes instead)
	*/
	class GODZ_API RenderDeviceObject : public GenericReferenceCounter
	{
	public:
		virtual ~RenderDeviceObject();

		// Subclasses should release their GPU resources, etc here
		virtual void Release() = 0;

		// Registry of all the RenderDeviceObjects. Allows us to handle
		// lost device events and insure we dont leak resources.
		typedef stdext::hash_map<RenderDeviceObject*,RenderDeviceObject*> RenderDeviceObjMap;
		static RenderDeviceObjMap mRenderObjectMap;

	protected:
		RenderDeviceObject();
	};

} //namespace GODZ

#endif
