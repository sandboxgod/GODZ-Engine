
#ifndef __PACKAGE_LIST_H__
#define __PACKAGE_LIST_H__

#include "GenericNode.h"
#include "GodzAtomics.h"
#include <hash_map>
#include <vector>

namespace GODZ
{
class GenericPackage;
class GenericObject;

/*
* vector containing a list of packages
*/
class GODZ_API PackageListVec
{
public:
	void Add(GenericPackage* p);
	size_t GetNumPackages();
	GenericPackage* operator[](size_t index);

private:
	std::vector<GenericPackage*> mPackages;
};

/*
* PackageList
*/
class GODZ_API PackageList
{
public:
	PackageList();

	//Creates a new package. First it ensures the package doesn't already exist!
	void AddPackage(GenericPackage* package);

	//Searches to determine if package exists- if not then one is created....
	GenericPackage* CreateOrFindPackage(const char* packageName, EPackageFlag flag = PF_GDZ);

	//Drops all packages loaded within memory.
	void DropPackages();

	void Unload(GenericPackage* package);

	//Searches for the package
	GenericPackage* FindPackageByName(HString packageName);
	bool FindPackage(GenericPackage* package);

	//Searches all registered classes to see if they own the object
	GenericPackage* GetPackage(GenericObject* obj);

	void GetPackagesOfType(GenericClass* type, PackageListVec& packages);
	void GetPackages(PackageListVec& list);

private:
	//Removes a package from this list....
	void Drop(GenericPackage* package);

	typedef stdext::hash_map<HashCodeID, GenericReference<GenericPackage> > PackageMap;
	PackageMap m_package;

	GodzAtomic		m_lock;
};



}


#endif //__PACKAGE_LIST_H__