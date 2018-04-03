
#include "HStringTable.h"
#include "FileWriter.h"
#include "FileReader.h"
#include "ResourceManager.h"


namespace GODZ
{

HStringTable::HStringTable()
: GenericSingleton<HStringTable>(*this)
{
}

void HStringTable::Clear()
{
	mMap.clear();
}

void HStringTable::AddString(HashCodeID hash, const char* name)
{
	StringMap::iterator iter;
	iter = mMap.find(hash);

	if (iter != mMap.end())
	{
		//increment the refcount....
		iter->second.refcount++;
	}
	else
	{
		//add a new entry
		HStringInfo info;
		info.refcount = 1;
		info.text = name;

		mMap[hash] = info;
	}
}

const char* HStringTable::GetString(HashCodeID hash)
{
	StringMap::iterator iter;
	iter = mMap.find(hash);

	if (iter != mMap.end())
	{
		return iter->second.text;
	}

	return NULL;
}

void HStringTable::RemoveString(HashCodeID hash)
{
	StringMap::iterator iter;
	iter = mMap.find(hash);

	if (iter != mMap.end())
	{
		iter->second.refcount--;
		if (iter->second.refcount == 0)
		{
			mMap.erase(iter);
		}
	}
}

void HStringTable::GetStrings(std::vector<HStringExportInfo>& strings)
{
	StringMap::iterator iter;
	for(iter = mMap.begin(); iter != mMap.end(); iter++)
	{
		HStringExportInfo e;
		e.text = iter->second.text;
		e.hash = iter->first;

		strings.push_back(e);
	}
}

void HStringTable::Load()
{
	rstring dir = GetBaseDirectory();
	dir += "\\Data\\stringtable.dat";

	if (FileExists(dir))
	{
		ResourceManager* resMngr = ResourceManager::GetInstance();

		FileReader reader(dir, &resMngr->GetResourceFactory());
		GDZArchive archive(&reader);
		Serialize(archive);
	}
}

void HStringTable::Save()
{
	rstring dir = GetBaseDirectory();
	dir += "\\Data\\stringtable.dat";

	FileWriter writer(dir);
	GDZArchive archive(&writer);
	Serialize(archive);
}

//We use a DB to save/load all the strings. But this method can be used for export/import
void HStringTable::Serialize(GDZArchive& ar)
{
	if (ar.IsSaving())
	{
		//get a count...
		size_t count = 0;

		StringMap::iterator iter;
		for(iter = mMap.begin(); iter != mMap.end(); iter++)
		{
			count++;
		}

		ar << count;

		for(iter = mMap.begin(); iter != mMap.end(); iter++)
		{
			HashCodeID hash = iter->first;
			ar << hash;
			ar << iter->second.text;
			ar << iter->second.refcount;
		}
	}
	else
	{
		size_t count;
		ar << count;

		for(size_t i=0;i<count;i++)
		{
			HashCodeID hash;
			ar << hash;

			HStringInfo info;
			ar << info.text;
			ar << info.refcount;

			//Add the hash again for each ref
			for(size_t j=0; j<info.refcount;j++)
			{
				AddString(hash, info.text);
			}
		}
	}
}

}