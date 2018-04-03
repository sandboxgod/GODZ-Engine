/* ===========================================================
	HStringTable.h

	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#if !defined(__HSTRINGTABLE__H__)
#define __HSTRINGTABLE__H__

#include "GenericSingleton.h"
#include "CoreFramework.h"
#include "GDZArchive.h"
#include "RString.h"
#include <hash_map>
#include <vector>

namespace GODZ
{
	struct GODZ_API HStringInfo
	{
		rstring text;
		size_t refcount;
	};

	//Export data about all the loaded HStrings
	struct GODZ_API HStringExportInfo
	{
		rstring text;
		HashCodeID hash;
	};

	/*
	* String map that should only be loaded within the editor, etc
	*/
	class GODZ_API HStringTable : public GenericSingleton<HStringTable>
	{
	public:
		HStringTable();
		void AddString(HashCodeID, const char* name);
		const char* GetString(HashCodeID hash);
		void RemoveString(HashCodeID hash);
		void Clear();

		void GetStrings(std::vector<HStringExportInfo>& strings);

		void Load();
		void Save();
		void Serialize(GDZArchive& ar);

		//--------------------------------------------------
		//Singleton interface
		//--------------------------------------------------

		//Return true if this singleton handles multiple thread calls concurrently
		virtual bool IsThreadSafe() { return true; }

	private:
		typedef stdext::hash_map<HashCodeID, HStringInfo> StringMap;
		StringMap mMap;
	};
}

#endif //__HSTRINGTABLE__H__
