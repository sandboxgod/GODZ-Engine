
#include "RenderQueue.h"
#include "MeshInstance.h"
#include <Windows.h>



namespace GODZ
{
	void SlotQueue::AddFreeSlot(const SlotIndex& id)
	{
		SynchronizedBlock synchronizedCodeBlock(mQueueLock);
		mFreeSlots.push(id);
	}

	SlotIndex SlotQueue::GetNextVisualID()
	{
		// Synchronized code block....
		{
			SynchronizedBlock synchronizedCodeBlock(mQueueLock);
			if (!mFreeSlots.empty())
			{
				VisualID id = mFreeSlots.front();
				mFreeSlots.pop();

				return id;
			}
		}

		// If we are generating a new id, we can safely assume that the
		// instance count will be 1.
		SlotIndex id;
		id.mInstanceCount = 1;
		id.mSlotNumber = mNextIndex++; 
		return id; 
	}

	//////////////////////////////////////////////////////////////////////////

	SlotIndex RenderQueue::GetNextMaterialIndex()
	{
		return mMaterialFreeSlots.GetNextVisualID();
	}

	SlotIndex RenderQueue::GetNextVisualIndex()
	{
		return mVisualFreeSlots.GetNextVisualID();
	}

	//////////////////////////////////////////////////////////////////////////


} //namespace GODZ