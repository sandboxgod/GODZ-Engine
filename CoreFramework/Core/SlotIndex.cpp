
#include "SlotIndex.h"

namespace GODZ
{
	SlotIndex::SlotIndex(const UInt64 value)
	{
		UInt64 mask = 0xFFFFFFFF;

		// Shift over 32 bits... compiler won't let us do it in
		// one go...
		mSlotNumber = value >> 31;
		mSlotNumber = mSlotNumber >> 1;

		mInstanceCount = value & mask;
	}


	UInt64 SlotIndex::GetUInt64() const
	{
		// Move SlotNumber up 32 bits...
		UInt64 slotNumber = mSlotNumber;
		UInt64 number = slotNumber << 31;
		number = number << 1;

		// Insert the Instance Number
		number = number | mInstanceCount;
		return number;
	}

}
