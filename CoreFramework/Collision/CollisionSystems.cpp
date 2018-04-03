

#include "CollisionSystems.h"
#include <CoreFramework/Core/Sort.h>
#include "PhysicsWorld.h"

namespace GODZ
{

void TraceResults::AddResult(const TraceResult& result)
{
	m_results.push_back(result); 
}

void TraceResults::InsertResult(const TraceResult& result)
{
	std::vector<TraceResult>::iterator iter;
	for(iter = m_results.begin(); iter != m_results.end(); iter++)
	{
		if (result.m_distance <= iter->m_distance)
		{
			m_results.insert(iter, result);
			return;
		}
	}

	m_results.push_back(result);
}

TraceResult& TraceResults::GetResult(size_t i)
{
	godzassert(i < m_results.size());
	return m_results[i];
}

void TraceResults::Sort()
{
	//for now uses bubble sort
	bubbleSort(m_results);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

PhysicsCollisionResult::PhysicsCollisionResult(const CollisionResult& r)
: m_hitLocation(r.m_hitLocation)
, m_normal(r.m_normal)
{
	m_id = r.model.GetID();
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void PhysicsCollisionResults::AddCollision(const PhysicsCollisionResult& result)
{
	size_t num = m_results.size();
	for(size_t i=0;i<num;i++)
	{
		if (result.m_id == m_results[i].m_id)
		{
			m_results[i] = result;
			return; //update to latest collision info then bail
		}
	}

	m_results.push_back(result);
}

size_t PhysicsCollisionResults::Size()
{
	return m_results.size();
}

PhysicsCollisionResult& PhysicsCollisionResults::GetResult(size_t i)
{
	return m_results[i];
}

void PhysicsCollisionResults::Clear()
{
	m_results.clear();
}



/////////////////////////////////////////////////////////////////////////////////////////////////

void SynchronizedCollisionList::AddCollision(const PhysicsCollisionResult& result)
{
	SynchronizedBlock lock(m_collisionListLock);
	size_t num = m_results.size();
	for(size_t i=0;i<num;i++)
	{
		if (result.m_id == m_results[i].m_id)
		{
			m_results[i] = result;
			return; //update to latest collision info then bail
		}
	}

	m_results.push_back(result);
}

size_t SynchronizedCollisionList::Size()
{
	size_t num = 0;

	SynchronizedBlock lock(m_collisionListLock);
	num = m_results.size();

	return num;
}

void SynchronizedCollisionList::GetResult(size_t i, PhysicsCollisionResult& result)
{
	SynchronizedBlock lock(m_collisionListLock);
	result = m_results[i];
}

void SynchronizedCollisionList::Clear()
{
	SynchronizedBlock lock(m_collisionListLock);
	m_results.clear();
}

void SynchronizedCollisionList::GetCopy(PhysicsCollisionResults& list)
{
	SynchronizedBlock lock(m_collisionListLock);
	list.m_results = m_results;
}


}