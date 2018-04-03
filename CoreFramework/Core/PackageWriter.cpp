
#include "PackageWriter.h"
#include "FileWriter.h"

using namespace GODZ;
using namespace std;

PackageWriter::PackageWriter()
{
}

bool PackageWriter::ContainsObject(GenericObject *obj)
{
	vector<GenericObject*>::iterator objIter;
	for(objIter=generics.begin();objIter!=generics.end();objIter++)
	{
		GenericObject *thisObj = (*objIter);
		if (thisObj==obj)
			return true;
	}

	return false;
}

bool PackageWriter::SavePackage(GenericPackage* package, const char* filename)
{
	this->package=package;
	FileWriter writer(filename);

	if ( writer.HasStream() && package->GetNumObjects() > 0)
	{
		size_t numObjects = package->GetNumObjects();
		for(size_t i=0;i<numObjects;i++)
		{
			GenericObject* obj = package->GetObjectAt(i);

			if (!ContainsObject(obj))
			{
				//First we write out all objects that belong to this package to the vector
				generics.push_back(obj);
			}
		}

		writer.WritePackage(package, generics);
		writer.Close();

		//cleanup...
		generics.clear();

		return true;
	}

	return false;
}
