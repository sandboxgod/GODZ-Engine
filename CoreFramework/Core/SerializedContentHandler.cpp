
#include "SerializedContentHandler.h"

using namespace GODZ;
using namespace std;


ObjectEntry::ObjectEntry()
: mClassHash(0)
, objOffset(0)
, iPkgIndex(0)
{
}

int SerializedContentHandler::GetObjectEntry(GenericObject* serializedObject)
{
	size_t num = serials.size();
	for(size_t i = 0; i < num; i++)
	{
		if (serials[i]==serializedObject)
		{
			return i;
		}
	}

	return -1;
}

int SerializedContent::GetObjectEntry(GenericObject* serializedObject)
{
	size_t num = serials.size();
	for(size_t i = 0; i < num; i++)
	{
		if (serials[i]==serializedObject)
		{
			return i;
		}
	}

	return -1;
}

