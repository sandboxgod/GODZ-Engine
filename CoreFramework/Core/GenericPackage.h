/* ===========================================================
	GenericPackage

	Represents a package (like Java, etc). Generic Objects are
	always associated with a package definition.

	Created Mar 15,'04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#include "CoreFramework.h"
#include <vector>
#include <hash_map>
#include "GodzAtomics.h"
#include "GenericObjectList.h"
#include "atomic_ref.h"
#include "GenericObject.h"
#include <Guiddef.h>

#if !defined(_GENERIC_PACKAGE_H_)
#define _GENERIC_PACKAGE_H_

namespace GODZ
{
	class ResourceObject;

	/*
	* Stores a list of package dependencies
	*/
	class GODZ_API PackageDependencies
	{
	public:
		void Reserve(size_t num);
		void Add(HashCodeID hash);
		size_t Size() const;
		HashCodeID Get(size_t index) const;
		bool Contains(HashCodeID hash);

	private:
		std::vector<HashCodeID> m_dependencies;
	};

	/*
	* Represents a collection of objects. Packages are intended to be 'static' after the resource loader creates one.
	* This way a package can be shared by multiple threads
	*/
	class GODZ_API GenericPackage : public GenericReferenceCounter
	{
	public:
		typedef stdext::hash_map<HashCodeID, GenericReference<ResourceObject> > ResourceList;


		//should only be used by Tools / Editor
		GenericPackage(const char* name, EPackageFlag flag = PF_GDZ);

		//can be used during runtime, etc
		GenericPackage(const char* name, HashCodeID hash, EPackageFlag flag = PF_GDZ);
		~GenericPackage();

		//should be invoked after package done loading
		void OnLoaded();

		//adds the hash of the package we are dependant on
		void AddDependency(HashCodeID hash);

		//Returns a list of all the packs we rely on
		const PackageDependencies& GetPackageDependencies();

		//Checks to see if we are dependant upon the hash value
		bool IsDependantOn(HashCodeID hash);

		void ReserveObjects(size_t num);

		//adds an object to this package
		void AddObject(const atomic_ptr<GenericObject> obj);
		
		//Adds object + associates it with the GUID
		void AddObject(const atomic_ptr<GenericObject> obj, GUID id);

		//returns number of objects owned by this package
		size_t GetNumObjects() { return m_obj.size(); }

		//returns the object at the desired index
		GenericObject* GetObjectAt(size_t index);
		atomic_ptr<GenericObject> GetReferenceAt(size_t index);

		bool FindObject(GenericObject* obj, size_t &index);
		bool FindObject(GenericObject* obj);
		GenericObject* FindObject(HashCodeID hash);
		atomic_ptr<GenericObject> FindObjectRef(HashCodeID hash);
		atomic_ptr<GenericObject> FindObjectRef(GenericObject* obj);
		GenericObject* FindObjectByGUID(GUID id);
		atomic_ptr<GenericObject> FindObjectRefByGUID(GUID id);

		// Searches the package for the Object. If it's a member, it returns the GUID for it.
		GUID GetGUIDForObject(GenericObject* obj);

		bool RemoveObject(GenericObject* obj);

		//Returns the hash code for this package
		HashCodeID GetPackageID()	{ return mHash; }

		void SetName(const char* newName);
		const char* GetName() const;

		EPackageFlag GetPackageFlag() const;
		void SetPackageFlag(EPackageFlag flag) { packageFlag = flag; }

		void GetObjectsOfType(GenericClass* type, GenericObjectList& objects);

		void Clear();

		void AddResource(ResourceObject* resource);
		ResourceObject* findResource(HashCodeID hash);
		void GetResourceIterators(GenericPackage::ResourceList::iterator& begin, GenericPackage::ResourceList::iterator& end);

	private:

		void Lock();
		void Unlock();

		struct PackageObject
		{
			atomic_ptr<GenericObject> mObject;
			GUID mId;
		};

		rstring						packageName;
		EPackageFlag				packageFlag;
		HashCodeID					mHash;

		//objects owned by this package
		typedef std::vector<PackageObject> EntityList;
		EntityList	m_obj;
		ResourceList				m_resourceList;


		//Packages we depend on.....
		PackageDependencies			m_dependencies;

		GodzAtomic					m_packageLock;
		
	};
}

#endif