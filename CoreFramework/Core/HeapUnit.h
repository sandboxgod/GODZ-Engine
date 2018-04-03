/* ===========================================================
	HeapUnit

	Memory Management Container

	Proverbs 3:9 "Honor the Lord with your wealth, with the
	firstfruits of all of your crops; then your barns will be
	filled to overflowing, and your vats will brim over with
	new wine."

	Created Oct 21, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__HEAP_UNIT__)
#define __HEAP_UNIT__

#include "Godz.h"

namespace GODZ
{
	//defines how many heap groups we want to define for each group of bytes
	static const unsigned int HEAP_GROUP_SIZE = 4 * 1024; 

	//forward decl
	class HeapUnit;

	//High level container for a memory chunk
	struct GODZ_API MemorySpace
	{
		unsigned char *m_pBuffer;
		size_t size;		//size of the heap
	};

	//Represents a memory block
	struct GODZ_API HeapBlock
	{
		size_t size;		//size of this memory block
		bool m_bFree;	   //is in use? 
	};

	//Manages a group of heap blocks
	struct GODZ_API HeapBlockGroup
	{
		HeapBlockGroup()
		{
			m_pHugeBlock=0;
			m_nUsedBytes=0;
			m_nFreeBlocks=0;
		}

		~HeapBlockGroup();

		//request data from this group
		//[in] bAllocNextAddr - Allocate to next address
		void* Allocate(size_t dataSize, HeapUnit *pUnit, bool bAllocNextAddr=0);

		//clears the memory
		void Clear();

		//releases the memory occupied at the address
		void  Free(size_t addr, HeapUnit *pUnit, bool bZeroMemory);

		//query to determine if this group has enough room for the requested bytes
		bool HasFree(size_t bytes, HeapUnit *pUnit);

		size_t m_nAddress;				//address of this group		
		size_t m_nUsedBytes;			//includes bytes used by memblocks- whether they or free or not
		HeapBlock *m_pHugeBlock;		//largest freed memory address we have avail within a block
		size_t m_nHugeBlockAddr;		//address of the huge block
		std::vector<HeapBlock*> m_pBlocks;
		size_t m_nFreeBlocks;			//# of free blocks
	};

	//Heap manager. Consists of Heap blocks, one huge memory chunk, and child
	//'units' that provides independant implementations for accessing data.
	class GODZ_API HeapUnit
	{
		friend HeapBlockGroup;

	public:
		//[in] bytes - size of the heap
		//[in] groupSize - size of each heap partition. The more partitions, the faster
		//memory access will be. However, this increases memory usage.
		//[in] bZeroMemory - zero out released memory
		//[in] bCanGrow - can add on another heap unit as needed
		//[in] bUseLibC - uses the C Library routines to manage memory
		HeapUnit(size_t bytes, size_t groupSize=HEAP_GROUP_SIZE, bool bCanGrow=true, bool bUseLibC=false);

		//removes this heap and all child heaps
		~HeapUnit();

		//Adds a child heap. Child heaps manages their own memory independantly.
		HeapUnit* AddChild(size_t bytes, size_t groupSize=HEAP_GROUP_SIZE);

		//Request data from this heap.
		void* Allocate(size_t dataSize);

		//frees an address from this heap
		void  Free(void* ptr);

		//returns the amount of memory occupied by heap groups and heap blocks
		size_t GetMemStats();

		//returns the number of heap groups used
		size_t GetNumGroups();

		//returns the number of heaps
		size_t GetNumHeaps();

		//returns amount of memory currently used by this heap. For instance, if the heap
		//size if only 2048 bytes and only 4 bytes is occupied- then 4 bytes is returned.
		//Child heaps are included in the query result.
		size_t GetUsedMemory();

		//returns true if this heap unit has the requested amount of
		//*contiguous* memory blocks free within this unit. The amount 
		//returned does not include the amount of freed blocks. This 
		//function does not account for dynamic memory.
		bool HasFreeBlocks(size_t bytes);

		//returns true if normal C library heap allocation is being used
		bool IsUsingLibC();

		//queries this heap unit whether the memory address is managed by this
		//heap or not (also includes child heaps)
		bool IsValidAddress(void *ptr);

		//Drops the current heap and creates a new one
		void Realloc();

		//set true if this heap should zero out released memory.
		void SetZeroMemory(bool bZero);

	protected:

		//returns the group associated with the address
		HeapBlockGroup* GetHeapFor(size_t address);

	private:
		//creates a child heap. Bytes is the amount of memory we should take
		//away from the parent.
		//HeapUnit(HeapUnit *parentHeap, size_t bytes, size_t groupSize);			
		void BuildHeapGroups();

		size_t m_nFirstAddress;			//beginning address for this heap (don't alter)
		size_t m_nNextFreeAddress;		//next free address
		size_t m_nGroupSize;			//size of each group		
		size_t m_nNumGroups;			//num of heap groups
		MemorySpace *m_pHeap;			//actual heap
		//HeapUnit* m_parent;			//parent heap
		HeapBlockGroup *m_pGroups;		//provides optimized memory access
		std::vector<HeapUnit*> m_pChildren; //child units - partitions this heap
		HeapUnit* m_pDymUnit;			//dynamic memory access - has own heap, no parent
		bool m_bZeroMemory;				//set to true to zero out released memory 
		bool m_bCanGrow;				//can add on additional memory
		bool m_bUseLibC;	//just use malloc/free functions
		size_t m_nUsedBytes; //used if we are using the C libraries for dynamic allocation
	};
}

#endif __HEAP_UNIT__