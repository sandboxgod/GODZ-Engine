/* ===========================================================
	MemoryMngr

	This header defines the global heaps used by the engine.
	All heaps used by this application are children of the
	global application heap.

	Created Oct 24, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__GLOBAL_HEAPS_H__)
#define __GLOBAL_HEAPS_H__

#include "HeapUnit.h"

namespace GODZ
{
	struct GODZ_API GlobalHeaps
	{
		static const size_t ONE_MB = 1024 * 1024;
		static const size_t ONE_KB = 1024;

		static HeapUnit *m_pAppHeap;		//global application heap
		static HeapUnit* m_pAABBTrees;
		static HeapUnit* m_pClassHeap;		//Memory allocated to Generic Classes
		static HeapUnit* m_pObjectHeap;		//Memory allocated to Generic Objects

		//releases all heaps
		static void ReleaseHeaps();
	};

	//creates heaps - called by the engine when it starts
	GODZ_API void CreateHeaps();
}

#endif __GLOBAL_HEAPS_H__