/* ===========================================================


	Created Nov 29, '09 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__GENERICOBJECTLIST_H__)
#define __GENERICOBJECTLIST_H__

#include "Godz.h"
#include <vector>

namespace GODZ
{
	class GenericObject;

	class GODZ_API GenericObjectList
	{
	public:
		void Add(GenericObject* obj);
		GenericObject* Get(size_t index);
		size_t GetNumObjects();
		GenericObject* operator[](size_t index);

	private:
		std::vector<GenericObject*> mList;
	};
}

#endif //__GENERICOBJECTLIST_H__