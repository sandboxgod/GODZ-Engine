/* ===========================================================
	RenderQueue.h

	Defines structures used for "Game" thread to communicate
	with "Render"

	Copyright (c) 2012
	========================================================== 
*/

#if !defined(__RENDERQUEUE__H__)
#define __RENDERQUEUE__H__

#include "Godz.h"
#include "SlotIndex.h"
#include "GodzAtomics.h"
#include <queue>

namespace GODZ
{
	/*
	* Stores a queue containing free slots
	*/
	class GODZ_API SlotQueue
	{
	public:
		SlotQueue()
			: mNextIndex(0)
		{
		}

		SlotIndex GetNextVisualID();
		void AddFreeSlot(const SlotIndex& id);

	private:
		GodzAtomic mQueueLock;
		AtomicInt mNextIndex;					// thread safe
		std::queue<SlotIndex> mFreeSlots;		// requires synchronized access
	};


	class GODZ_API RenderQueue
	{
	public:
		SlotIndex GetNextVisualIndex();
		SlotIndex GetNextMaterialIndex();

		SlotQueue mMaterialFreeSlots;
		SlotQueue mVisualFreeSlots;
	};
}

#endif //__RENDERQUEUE__H__
