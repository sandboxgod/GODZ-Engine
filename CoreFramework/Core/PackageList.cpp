
#include "PackageList.h"
#include "GenericPackage.h"

namespace GODZ
{

void PackageListVec::Add(GenericPackage* p)
{
	mPackages.push_back(p);
}

size_t PackageListVec::GetNumPackages()
{
	return mPackages.size();
}

GenericPackage* PackageListVec::operator[](size_t index)
{
	return mPackages[index];
}

//////////////////////////////////////////////////////////////////////////////////////////
PackageList::PackageList()
{
}

GenericPackage* PackageList::GetPackage(GenericObject* obj)
{
	SynchronizedBlock lock(m_lock);

	PackageMap::iterator iter;
	for (iter = m_package.begin(); iter != m_package.end(); iter++)
	{
		GenericPackage* temp = iter->second;
		if ( temp != NULL && temp->FindObject(obj) )
			return temp;
	}

	return NULL;
}

void PackageList::AddPackage(GenericPackage* gp)
{
	godzassert(FindPackageByName(gp->GetPackageID()) == false);		//make sure the package has not already been registered

	SynchronizedBlock lock(m_lock);
	m_package[gp->GetPackageID()] = gp;
}

GenericPackage* PackageList::CreateOrFindPackage(const char* packageName, EPackageFlag flag)
{
	//FindPackageByName is already synchronized so no need to put a lock above it...
	GenericPackage* p = FindPackageByName(packageName);
	if (p != NULL)
	{
		return p;
	}

	SynchronizedBlock lock(m_lock);
	p = new GenericPackage(packageName, flag);
	m_package[p->GetPackageID()] = p;

	return p;
}

void PackageList::GetPackages(PackageListVec& list)
{
	while (!m_lock.Set());
	PackageMap::iterator iter;
	for (iter = m_package.begin(); iter != m_package.end(); iter++)
	{
		GenericPackage* temp = iter->second;
		if (temp != NULL)
			list.Add(temp);
	}

	m_lock.UnSet();
}

void PackageList::Unload(GenericPackage* package)
{
	SynchronizedBlock lock(m_lock);
	Drop(package);
}

//unloading a package...
void PackageList::Drop(GenericPackage* package)
{
	//We do not need to set an atomic lock on this function, we assume it's already set
	godzassert(m_lock.IsSet());

	bool listChanged = false;

	//keep dropping children....
	do
	{
		listChanged = false;

		//find all packages that refer to this one.... We need to release those 1st
		PackageMap::iterator iter;
		for (iter = m_package.begin(); iter != m_package.end(); iter++)
		{
			GenericPackage* temp = iter->second;
			if (temp != NULL && temp->IsDependantOn(package->GetPackageID()) )
			{
				Drop(temp);
				listChanged = true;
				break;
			}
		}
	} while (listChanged);


	//Now we can release the desired package
	PackageMap::iterator iter;
	for (iter = m_package.begin(); iter != m_package.end(); iter++)
	{
		GenericPackage* temp = iter->second;
		if (temp == package)
		{
			m_package.erase(iter);
			break;
		}
	}
}


void PackageList::DropPackages()
{
	SynchronizedBlock lock(m_lock);

	bool listChanged = false;

	do 
	{
		listChanged = false;

		PackageMap::iterator iter;
		for (iter = m_package.begin(); iter != m_package.end(); iter++)
		{
			GenericPackage* temp = iter->second;

			if (temp != NULL)
			{
				Drop(temp);
				listChanged = true;
				break;
			}
		}
	} while (listChanged);
}


bool PackageList::FindPackage(GenericPackage* package)
{
	SynchronizedBlock lock(m_lock);
	PackageMap::iterator iter;
	for (iter = m_package.begin(); iter != m_package.end(); iter++)
	{
		GenericPackage* temp = iter->second;
		if (temp == package)
		{
			return true;
		}
	}

	return false;
}

GenericPackage* PackageList::FindPackageByName(HString packageName)
{
	SynchronizedBlock lock(m_lock);
	HashCodeID id = packageName.getHashCode();
	PackageMap::iterator iter = m_package.find( id );
	if( iter != m_package.end() )
		return iter->second;

	return NULL;
}

void PackageList::GetPackagesOfType(GenericClass* type, PackageListVec& packages)
{
	SynchronizedBlock lock(m_lock);
	PackageMap::iterator iter;
	for (iter = m_package.begin(); iter != m_package.end(); iter++)
	{
		GenericPackage* temp = iter->second;

		if (temp != NULL)
		{
			GenericObjectList list;
			temp->GetObjectsOfType(type, list);

			if (list.GetNumObjects() > 0)
			{
				packages.Add(temp);
			}
		}
	}
}

} //namespace GODZ

