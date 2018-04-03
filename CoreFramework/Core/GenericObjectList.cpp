
#include "GenericObject.h"
#include "GenericObjectList.h"

namespace GODZ
{
	void GenericObjectList::Add(GenericObject* obj)
	{
		mList.push_back(obj);
	}

	GenericObject* GenericObjectList::Get(size_t index)
	{
		return mList[index];
	}

	size_t GenericObjectList::GetNumObjects()
	{
		return mList.size();
	}

	GenericObject* GenericObjectList::operator[](size_t index) 
	{ 
		godzassert(index < mList.size());
		return mList[index] ; 
	}
}
