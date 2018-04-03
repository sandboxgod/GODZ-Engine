
#include "PackageReader.h"
#include "GenericPackage.h"
#include "FileReader.h"

using namespace GODZ;

PackageReader::PackageReader(ResourceFactory* factory)
: package(NULL)
, m_factory(factory)
{
}

void PackageReader::ReadPackageDependencies(const char* filename, PackageDependencies& dependencies)
{
	FileReader reader(filename, m_factory);
	reader.ReadPackageDependencies(dependencies);
	reader.Close();	
}

void PackageReader::ReadPackage(const char* filename)
{
	FileReader reader(filename, m_factory);
	package = reader.ReadPackage();
	reader.Close();
}

GenericPackage* PackageReader::GetPackage()
{
	return package;
}