/* ===========================================================
	GenericObjData

	Generic object lists

	"Plans fail for lack of counsel, but with many advisers
	they succeed." - Proverbs 15:22

	Created Feb 10, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__GENERICOBJDATA_H__)
#define __GENERICOBJDATA_H__

#include "Godz.h"
#include "IWriter.h"
#include "GenericPackage.h"
#include "PackageList.h"
#include "GenericNode.h"
#include <CoreFramework/Reflection/GenericClass.h>

namespace GODZ
{

	//Stores global engine data - private to Engine.DLL
	struct GODZ_API GenericObjData
	{
		static size_t						counter;						//current class number

		static const udword					MAX_OBJECT_SIZE = 128;			//maximum number of classes that can be regiestered
		static atomic_ptr<GenericClass>	m_pClass[MAX_OBJECT_SIZE];			//list of class instances

		static GenericPackage*				m_pCurrentPackage;				//Current object package (usually set to GODZ)
		static PackageList					m_packageList;					//global list of packages available to main thread

		static bool							mIsClassListBuilt;				//set to true after the class list has been updated

		static atomic_ptr<GenericClass>		GetClass(size_t index)
		{
			return m_pClass[index];
		}

		static size_t GetNumClasses()
		{
			return counter;
		}
	};
}

#endif //__GENERICOBJDATA_H__
