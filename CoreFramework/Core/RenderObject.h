/* ===========================================================
	RenderObject.h


	Copyright (c) 2012, Richard Osborne
	========================================================== 
*/

#if !defined(__RenderObject__H__)
#define __RenderObject__H__

#include "Godz.h"
#include "SlotIndex.h"

namespace GODZ
{
	class GODZ_API RenderObject
	{
	public:
		void SetVisualID(VisualID id) { mID = id; }
		VisualID GetVisualID() { return mID; }

	private:
		VisualID mID;
	};
}

#endif //__RenderObject__H__
