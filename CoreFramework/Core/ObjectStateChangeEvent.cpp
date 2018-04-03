/* ===========================================================
ObjectStateChangeEvent

Copyright (c) 2009, Richard Osborne
=============================================================
*/

#include "ObjectStateChangeEvent.h"


namespace GODZ
{

ObjectStateChangeEvent::ObjectStateChangeEvent(ObjectStateChangeEventID id)
: mType(id)
{
}

rstring ObjectStateChangeEvent::getDebug()
{
	return GetString("Event ID: %d", mType);
}

/////////////////////////////////////////////////////////////////////////////

ObjectStateChangeEventList::ObjectStateChangeEventList()
{
}

void ObjectStateChangeEventList::Add(ObjectStateChangeEvent* ev)
{
	
	if (mQuickIndex[ev->mType].size() > 0)
	{
		//iterates through all the known entries for this type
		size_t numElements = mQuickIndex[ev->mType].size();
		for(size_t i = 0; i < numElements;i++)
		{
			//get the location of the Other event...
			size_t ind = mQuickIndex[ev->mType][i];
			if ( ind > -1 && ev->isMoreRecent(mList[ind]) )
			{
				//free old event
				delete mList[ind];

				if (ev->shouldReplace())
				{
					mList[ind] = ev;
					return;
				}
				else
				{
					mList[ind] = NULL;

					//set the quick index to -1 since this slot is now null...
					mQuickIndex[ev->mType][i] = -1;
				}
			}
		}
	}
	

	mList.push_back(ev);

	//store where the particular event was stored in the table...
	mQuickIndex[ev->mType].push_back(mList.size() - 1);
}

size_t ObjectStateChangeEventList::GetNumObjects()
{
	return mList.size();
}

ObjectStateChangeEvent* ObjectStateChangeEventList::operator[](size_t index)
{
	return mList[index];
}

void ObjectStateChangeEventList::Clear()
{
	mList.clear();

	
	for(size_t i = 0; i < ObjectStateChangeEventID_MAX; i++)
	{
		mQuickIndex[i].clear();
	}
	
}

////////////////////////////////////////////////////////////////////////

bool ObjectStateEventCollection::isMoreRecent(ObjectStateChangeEvent* other)
{
	godzassert(other->getType() == ObjectStateChangeEventID_Collection);
	ObjectStateEventCollection* eventCollection = static_cast<ObjectStateEventCollection*>(other);

	//take ALL of the events that belongs to the old collection and move them here.
	//We will automatically sort through the events, dropping old ones, etc
	size_t numEvents = eventCollection->mList.GetNumObjects();
	for(size_t i=0; i < numEvents; i++)
	{
		ObjectStateChangeEvent* stateEvent = eventCollection->mList[i];

		mList.Add(stateEvent);
	}

	//we must destroy old event; replace it with ourself
	return true;
}
}