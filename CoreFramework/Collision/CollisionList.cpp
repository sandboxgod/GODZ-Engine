
#include "CollisionList.h"

using namespace GODZ;

CollisionList::CollisionList()
: m_bBailOnFirstContact(false)
, m_world(NULL)
{
	m_hitResults.reserve(MAX_COLL_POLYS);
}

CollisionList::~CollisionList()
{
}

void CollisionList::OnCollisionOccured(CollisionResult& result)
{
	bool found = false;
	std::vector<CollisionResult>::iterator iter;
	for(iter = m_hitResults.begin(); iter != m_hitResults.end(); iter++)
	{
		if (result.m_distance <= iter->m_distance)
		{
			m_hitResults.insert(iter, result);
			found = true;
			break;
		}
	}

	if (!found)
	{
		m_hitResults.push_back(result);
	}
}

size_t CollisionList::GetNumResults()
{
	return m_hitResults.size();
}

CollisionResult* CollisionList::GetClosestResult()
{
	if (m_hitResults.size() > 0)
		return &m_hitResults[0];

	return NULL;
}

CollisionResult& CollisionList::operator[](size_t index)
{
	godzassert(index < m_hitResults.size());
	return m_hitResults[index];
}
