
#include "MemoryMngr.h"
#include <CoreFramework/Collision/AABBTree.h>
#include <crtdbg.h>
#include "GenericObjData.h"
#include "GenericObject.h"
#include <CoreFramework/Reflection/GenericClass.h>


using namespace GODZ;


HeapUnit* GlobalHeaps::m_pAppHeap   = 0;
HeapUnit* GlobalHeaps::m_pAABBTrees = 0;
HeapUnit* GlobalHeaps::m_pClassHeap = 0;
HeapUnit* GlobalHeaps::m_pObjectHeap = 0;


void GODZ::CreateHeaps()
{
	if (GlobalHeaps::m_pAppHeap==0)
	{
		//GlobalHeaps::m_pAppHeap   = new HeapUnit(GlobalHeaps::ONE_MB * 4, GlobalHeaps::ONE_KB); //1 KB, 128 byte separators
		GlobalHeaps::m_pAppHeap   = new HeapUnit(GlobalHeaps::ONE_KB, 128);
		//GlobalHeaps::m_pAABBTrees = GlobalHeaps::m_pAppHeap->AddChild(sizeof(AABBTree) * 6, sizeof(AABBTree));
		GlobalHeaps::m_pClassHeap = GlobalHeaps::m_pAppHeap->AddChild(sizeof(GenericClass) * GenericObjData::MAX_OBJECT_SIZE);
		GlobalHeaps::m_pObjectHeap = GlobalHeaps::m_pAppHeap->AddChild(GlobalHeaps::ONE_MB);

		//_CrtSetBreakAlloc(3836); //DEBUGGING - MEMORY LEAK
	}
}


void GlobalHeaps::ReleaseHeaps()
{
	if (m_pAppHeap)
	{
		delete m_pAppHeap;
		m_pAppHeap=0;
	}
}

