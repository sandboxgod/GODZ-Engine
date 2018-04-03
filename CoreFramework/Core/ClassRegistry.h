/* ===========================================================
	ClassRegistry

	"Plans fail for lack of counsel, but with many advisers
	they succeed." - Proverbs 15:22

	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#if !defined(__CLASSREGISTRY_H__)
#define __CLASSREGISTRY_H__

#include "Godz.h"
#include "atomic_ref.h"

namespace GODZ
{
	class GenericClass;

	class GODZ_API ClassRegistry
	{
	public:
		static atomic_ptr<GenericClass> findClass(HashCodeID hash);
	};
}

#endif //__CLASSREGISTRY_H__