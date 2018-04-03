/* ===========================================================


	Copyright (c) 2012, Richard Osborne
	========================================================== 
*/

#if !defined(_SLOTINDEX_)
#define _SLOTINDEX_

#include "Godz.h"


namespace GODZ
{
	// This represents a Handle that can be used to refer to an object
	struct GODZ_API SlotIndex
	{
		unsigned int mSlotNumber;				// array index location
		unsigned int mInstanceCount;			// each time a new object is added

		SlotIndex()
			: mSlotNumber(0)
			, mInstanceCount(0)
		{
		}

		SlotIndex(const UInt64 value);

		// Note: 0 is a valid value for mSlotNumber
		bool IsValid() const
		{
			return (mInstanceCount > 0);
		}

		bool operator==(const SlotIndex& other) const
		{
			return mSlotNumber == other.mSlotNumber && mInstanceCount == other.mInstanceCount;
		}

		bool operator!=(const SlotIndex& other) const
		{
			return !(mSlotNumber == other.mSlotNumber && mInstanceCount == other.mInstanceCount);
		}

		// Returns a single UInt64 value that combines the slot # & instance count
		UInt64 GetUInt64() const;
	};

	typedef SlotIndex VisualID;
	typedef SlotIndex EntityID;
}

#endif