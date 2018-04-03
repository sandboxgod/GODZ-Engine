/* ==============================================================
	IAnimNotify


	Created Nov 22, '04 by Richard Osborne
	Copyright (c) 2010
	=============================================================
*/

#if !defined(__ANIM_NOTIFY_H__)
#define __ANIM_NOTIFY_H__

namespace GODZ
{
	class SkelMeshInstance;

	//pure virtual interface for animation change subscribers 
	class GODZ_API IAnimNotify
	{
	public:
		//notification that the animation has switched
		virtual void NotifyUpdateFrame(SkelMeshInstance*)=0;

		//Only GenericObjects can use this interface (for serialization)
		virtual GenericObject* GetInstance()=0;
	};
}

#endif __ANIM_NOTIFY_H__