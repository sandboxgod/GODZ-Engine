
#include "HeapUnit.h"

using namespace GODZ;
using namespace std;


HeapUnit::HeapUnit(size_t bytes, size_t groupSize, bool bCanGrow, bool bUseLibC)
{
	m_pHeap=0;
//	m_parent=0;
	m_nUsedBytes=0;
	m_pGroups=0;
	m_pDymUnit=0;
	m_bCanGrow=bCanGrow;
	m_bZeroMemory=true;
	m_nGroupSize=groupSize;
	this->m_bUseLibC = bUseLibC;

	//The group size must be less than number of bytes
	godzassert(m_nGroupSize <= bytes);

	//allocate memory...
	m_pHeap = new MemorySpace;

	if (!m_bUseLibC)
	{
		if (bytes>0)
		{
			m_pHeap->m_pBuffer = (unsigned char*)malloc(bytes);

			if (m_pHeap->m_pBuffer == 0)
			{
				DisplayMessage("App Error", "Not enough memory available to run this application.");
				//system.exit(1);
			}

			m_nFirstAddress = (size_t)m_pHeap->m_pBuffer;
		}
		else
		{
			m_pHeap->m_pBuffer=0;
			m_nFirstAddress=0;
		}
	}
	
	//m_nFirstFreeAddress = m_nFirstAddress + bytes; //init to last address
	m_pHeap->size = bytes;

	if (!m_bUseLibC)
	{
		BuildHeapGroups();
	}
}

/*HeapUnit::HeapUnit(HeapUnit *parentHeap, size_t bytes, size_t groupSize)
{
	m_pHeap=0;
	m_pGroups=0;
	m_pDymUnit=0;
	m_parent=parentHeap;
	m_bCanGrow=parentHeap->m_bCanGrow;
	m_bZeroMemory=parentHeap->m_bZeroMemory;
	this->m_nGroupSize=groupSize;

	m_pHeap = new MemorySpace;
	m_pHeap->size = bytes;

	//need to set pointer to parent heap's buffer where we can start
	m_nFirstAddress = parentHeap->m_nNextFreeAddress;
	m_nNextFreeAddress = m_nFirstAddress;
	m_pHeap->m_pBuffer = &parentHeap->m_pHeap->m_pBuffer[m_nFirstAddress];
	//m_nFirstFreeAddress = m_nFirstAddress + bytes; //init to last address

	//copy over heap groups
	m_nNumGroups = m_pHeap->size / m_nGroupSize;
	m_pGroups = parentHeap->GetHeapFor(parentHeap->m_nNextFreeAddress);
}*/

HeapUnit::~HeapUnit()
{
	if (m_pHeap)
	{
		//there is only one chunk to free, unless this unit has it's own heap
		//if (!this->m_parent)
		//{

		if (m_pHeap->m_pBuffer)
			free(m_pHeap->m_pBuffer);
		
		delete[] m_pGroups;
		//}

		delete m_pHeap;			
	}

	releaseVector<HeapUnit>(this->m_pChildren);

	if (m_pDymUnit)
	{
		delete m_pDymUnit;
	}
}

HeapUnit* HeapUnit::AddChild(size_t bytes, size_t groupSize)
{
	HeapUnit* pUnit = new HeapUnit(bytes, groupSize, m_bCanGrow, m_bUseLibC);
	if (pUnit)
	{
		pUnit->m_bZeroMemory = this->m_bZeroMemory;
		m_pChildren.push_back(pUnit);
		return pUnit;
	}

	return 0;
}


void* HeapUnit::Allocate(size_t dataSize)
{
	if (m_bUseLibC)
	{
		void* pData = malloc(dataSize);
		if (pData)
		{
			m_nUsedBytes += dataSize;
			return pData;
		}

		return 0;
	}

	void *pData = 0;

	if (HasFreeBlocks(dataSize))
	{
		//get the heap group for the next available address
		HeapBlockGroup* group = GetHeapFor(this->m_nNextFreeAddress);
		if (group) //&& group->HasFree(dataSize,this))
		{
			//tell the heap group to allocate memory using this heap
			pData = group->Allocate(dataSize, this, true);
			if (pData)
				return pData;
		}
	}

	//find a heap group that has the free bytes
	for(unsigned int i=0;i<m_nNumGroups;i++)
	{
		if (m_pGroups[i].HasFree(dataSize,this))
		{
			pData = m_pGroups[i].Allocate(dataSize, this, 0);
			if (pData)
			{
				return pData;
			}
		}
	}

	//we are out of memory, see if we can use a dynamic heap
	if (m_bCanGrow)
	{
		if (m_pDymUnit)
		{
			pData = m_pDymUnit->Allocate(dataSize);
			if (pData)
			{
				return pData;
			}
		}
		else
		{
			//add a new heap...
			m_pDymUnit = new HeapUnit(m_pHeap->size, m_nGroupSize, m_bCanGrow);
			m_pDymUnit->m_bZeroMemory = m_bZeroMemory;
			if (m_pDymUnit)
			{
				return m_pDymUnit->Allocate(dataSize);
			}
		}
	}

	return 0;
}

void HeapUnit::BuildHeapGroups()
{
	m_nNextFreeAddress=m_nFirstAddress;

	//compute the # of heap regions we will need to help sort memory requests
	m_nNumGroups = m_pHeap->size / m_nGroupSize;
	m_pGroups = new HeapBlockGroup[m_nNumGroups];
	size_t addr = m_nFirstAddress;
	for(size_t i=0; i<m_nNumGroups; i++)
	{
		m_pGroups[i].m_nAddress = addr;
		//m_pGroups[i].m_nNextAddress = addr;
		addr += m_nGroupSize;
	}
}

HeapBlockGroup* HeapUnit::GetHeapFor(size_t address)
{
	if ( (address < m_nFirstAddress) || (address > m_nFirstAddress + this->m_pHeap->size))
	{
		//invalid heap request - the address is out-of-range of this heap unit
		return 0;
	}

	//compute the heap group this address belongs to
	size_t blockNum = (address - m_nFirstAddress) / m_nGroupSize;
	return &m_pGroups[blockNum];
}

size_t HeapUnit::GetMemStats()
{
	size_t mem = 0;

	if (m_pDymUnit)
	{
		mem += m_pDymUnit->GetMemStats();
	}

	//include the number of groups
	mem += sizeof(HeapBlockGroup) * m_nNumGroups;

	for(size_t i=0;i<this->m_nNumGroups;i++)
	{
		mem += sizeof(HeapBlock) * this->m_pGroups[i].m_pBlocks.size();
	}	

	return mem;
}

size_t HeapUnit::GetNumGroups()
{
	return this->m_nNumGroups;
}

size_t HeapUnit::GetNumHeaps()
{
	size_t count = m_pChildren.size();

	for(size_t i=0;i<count;i++)
	{
		HeapUnit* pUnit = m_pChildren[i];
		count += pUnit->GetNumHeaps();
	}

	HeapUnit* node = this->m_pDymUnit;
	while(node)
	{
		count += node->GetNumHeaps();
		node = node->m_pDymUnit;		
		count++;
	}

	return count;
}

size_t HeapUnit::GetUsedMemory()
{
	size_t mem = 0;

	if (this->m_pDymUnit)
	{
		mem += m_pDymUnit->GetUsedMemory();
	}

	for(size_t i=0;i<this->m_nNumGroups;i++)
	{
		//bytes currently in use
		mem += this->m_pGroups[i].m_nUsedBytes;
	}

	vector<HeapUnit*>::iterator heapIter;
	for(heapIter = m_pChildren.begin();heapIter!=m_pChildren.end();heapIter++)
	{
		mem += (*heapIter)->GetUsedMemory();
	}

	mem += m_nUsedBytes; //if we using c library allocation routines

	return mem;
}

bool HeapUnit::HasFreeBlocks(size_t bytes)
{
	if (this->m_bUseLibC)
	{
		return true; //TODO: should look at GlobalMemory()
	}

	size_t lastAddress = m_nFirstAddress + this->m_pHeap->size;
	return m_nNextFreeAddress + bytes <= lastAddress;
}

bool HeapUnit::IsUsingLibC() 
{ 
	return m_bUseLibC; 
}

void HeapUnit::Free(void* ptr)
{
	if (m_bUseLibC)
	{
		free(ptr);
		return;
	}

	if (m_pDymUnit && m_pDymUnit->IsValidAddress(ptr))
	{
		m_pDymUnit->Free(ptr);
		return;
	}

	size_t addr = (size_t)ptr;
	HeapBlockGroup* pGroup = GetHeapFor(addr);

	if (!pGroup)
	{
		godzassert(0); //the pointer does not belong to this heap unit
		return;
	}

	pGroup->Free(addr, this, m_bZeroMemory);

}

bool HeapUnit::IsValidAddress(void *ptr)
{
	if (m_bUseLibC)
	{
		return true;
	}

	if (m_pDymUnit && m_pDymUnit->IsValidAddress(ptr))
		return true;

	size_t addr = (size_t)ptr;
	HeapBlockGroup* pGroup = GetHeapFor(addr);
	return pGroup != 0;
}


void HeapUnit::Realloc()
{
	if (m_pDymUnit)
	{
		m_pDymUnit->Realloc();
	}

	for(size_t i=0;i<this->m_nNumGroups;i++)
	{
		m_pGroups[i].Clear();
	}
}

void HeapUnit::SetZeroMemory(bool bZero)
{
	this->m_bZeroMemory = bZero;
}

//////////////////////////////////////////////////////////

HeapBlockGroup::~HeapBlockGroup()
{
	releaseVector<HeapBlock>(m_pBlocks);
}

void* HeapBlockGroup::Allocate(size_t dataSize, HeapUnit *pUnit, bool bAllocNextAddr)
{
	size_t addr=this->m_nAddress;

	//find a free memory block
	HeapBlock* best=0;
	size_t bestAddr=0;

	
	if (m_pHugeBlock && m_pHugeBlock->size == dataSize)
	{
		m_pHugeBlock->m_bFree = 0;
		m_nFreeBlocks--;
		m_nUsedBytes += m_pHugeBlock->size;
		m_pHugeBlock=0;
		return &pUnit->m_pHeap->m_pBuffer[m_nHugeBlockAddr - pUnit->m_nFirstAddress];
	}

	if (m_nFreeBlocks > 0)
	{
	vector<HeapBlock*>::iterator blockIter;
	for(blockIter=this->m_pBlocks.begin();blockIter != m_pBlocks.end();blockIter++)
	{
		HeapBlock* pBlock = *blockIter;
		if(pBlock->m_bFree )
		{
			if (pBlock->size == dataSize)
			{
				pBlock->m_bFree=0;
				if (m_pHugeBlock == pBlock)
				{
					//we need to find another huge block
					m_pHugeBlock=0;
				}

				m_nFreeBlocks--;
				m_nUsedBytes += pBlock->size;

				return &pUnit->m_pHeap->m_pBuffer[addr - pUnit->m_nFirstAddress];
			}
			else if (pBlock->size > dataSize)
			{
				//see if this is closer than best
				if (best && pBlock->size - dataSize < best->size - dataSize)
				{
					best = pBlock;
					bestAddr=addr;
				}
				else if (!best)
				{
					best = pBlock;
					bestAddr=addr;
				}
			}
		}

		addr += pBlock->size; //figure out which address we at...
	}
	} //if free blocks

	//make sure we have enough free memory - sometimes we might get a mem request
	//right at the edge of the memory chunk. like 75640 when only 76600 avail and 
	//the requested # of bytes overruns the memory chunk --> 75640 + requested > chunk->size
	if ( (bAllocNextAddr || m_nUsedBytes < pUnit->m_nGroupSize)		
		&& (dataSize + pUnit->m_nNextFreeAddress  <= pUnit->m_nFirstAddress + pUnit->m_pHeap->size) 
		)
	{
		//allocate a new node
		HeapBlock* pBlock = new HeapBlock;
		pBlock->m_bFree=0;
		pBlock->size = dataSize;
		m_pBlocks.push_back(pBlock);

		//get the index into the memory chunk we want to return
		size_t index = pUnit->m_nNextFreeAddress - pUnit->m_nFirstAddress;
		void *pData = &pUnit->m_pHeap->m_pBuffer[index];
		pUnit->m_nNextFreeAddress += dataSize; //advance free memory ptr

		m_nUsedBytes += dataSize;
		return pData;
	}
	else if (best)
	{
		best->m_bFree = 0;
		m_nFreeBlocks--;
		m_nUsedBytes += best->size;
		return &pUnit->m_pHeap->m_pBuffer[bestAddr - pUnit->m_nFirstAddress];
	}
	
	return 0;
}

void HeapBlockGroup::Clear()
{
	this->m_pHugeBlock=0;
	this->m_nFreeBlocks = 0;
	this->m_nUsedBytes = 0;

	releaseVector<HeapBlock>(m_pBlocks);
}

void  HeapBlockGroup::Free(size_t addrToFree, HeapUnit *pUnit, bool bZeroMemory)
{
	//i might put in an algorithm to quickly find a heap block but for now...
	size_t addr=this->m_nAddress;

	//find a free memory block
	vector<HeapBlock*>::iterator blockIter;
	for(blockIter=this->m_pBlocks.begin();blockIter != m_pBlocks.end();blockIter++)
	{
		HeapBlock* pBlock = *blockIter;
		if (addr == addrToFree)
		{
			if (pBlock->m_bFree)
			{
				godzassert(0); //this heap block has already been freed!
			}

			pBlock->m_bFree = true;
			m_nUsedBytes -= pBlock->size;
			m_nFreeBlocks++;

			if (m_pHugeBlock)
			{
				if (pBlock->size > m_pHugeBlock->size)
				{
					m_pHugeBlock = pBlock;
					m_nHugeBlockAddr=addr;
				}
			}
			else
			{
				m_pHugeBlock = pBlock;
				m_nHugeBlockAddr=addr;
			}

			if (bZeroMemory)
			{
				godzassert(0); //need to write a platform indepent zeromemory or something
				//ZeroMemory(&pUnit->m_pHeap->m_pBuffer[addr - pUnit->m_nFirstAddress], pBlock->size);
			}
		}

		addr += pBlock->size; //figure out which address we at...
	}
}

bool HeapBlockGroup::HasFree(size_t bytes, HeapUnit *pUnit)
{
	//return true if the biggest freed memory block can store
	//the requested bytes...
	if (m_pHugeBlock)
	{
		return (m_pHugeBlock->size >= bytes);
	}
	else if (m_nFreeBlocks > 0)
	{
		vector<HeapBlock*>::iterator blockIter;
		for(blockIter=this->m_pBlocks.begin();blockIter != m_pBlocks.end();blockIter++)
		{
			HeapBlock* pBlock = *blockIter;
			if (pBlock->m_bFree && pBlock->size >= bytes)
			{
				return true;
			}
		}
	}

	//see if we have some contiguous bytes at the end of this group. this method
	//is unreliable so we do this query last (it's unreliable because a heap group
	//is capable of storing a data size beyond it's group size)
	if (bytes <= (pUnit->m_nGroupSize - m_nUsedBytes))
	{
		return true;
	}	

	return 0;
}