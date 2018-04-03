/* ===========================================================
	Metaclass for implementations that manipulate
	Serialized content.

	Created Feb 28, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_SERIALIZED_CONTENT_HANDLER_H_)
#define _SERIALIZED_CONTENT_HANDLER_H_

#include "GenericPackage.h"

namespace GODZ
{
	//FEBA is an acronym for an old game I created!
	static const ulong MAGIC_NUMBER = 0xFEBA;

	//Main file header
	struct PackageHeader
	{
		int iFileVersion;	//file version
		ulong magic;		//magic number for GODZ packages
		size_t objects;		//number of objects serialized within this package
		ulong objTblOffset;	//object table offset within file
		int names;			//number of strings going out
		ulong nameTblOffset;//offset within file where the name table starts!
		int packages;		//number of packages referenced by the objects within file
		ulong pkgTblOffset;//offset within file where the package table starts!
		int numResources;	//number of resources
		ulong resourceOffset;	//resource table offset within file
	};

	//Entry within file- represents a serialized object
	struct ObjectEntry
	{
		HashCodeID mClassHash;		//class hash ID
		int iPkgIndex;				//index within package array. Indicates package this object is from
		ulong objOffset;			//offset within file where object definition exists

		ObjectEntry();
	};

	//Represents a generic package. Packages are either 'imported' or 'exported'.
	//Export packages are simply objects we are serializing. Import packages are
	//packages that we need to import in order to use an object from that file.
	struct PackageEntry
	{
		HashCodeID mHash;	//package hash ID
		EPackageFlag flag;	//flag indicating whether this is a DLL, etc
		bool bExport;		//Indicates whether this package is an export/import.
	};

	class GODZ_API SerializedContentHandler
	{
	public:
		//Returns serialized ID # assigned to the object (IDs are assigned after object
		//has been serialized).
		int GetObjectEntry(GenericObject* serializedObject);
		GenericObject* GetSerializable(int serialID) { return serials[serialID]; }

		std::vector<GenericObject*> serials;		//contains serialized objects
	};

	class GODZ_API SerializedContent
	{
	public:
		//Returns serialized ID # assigned to the object (IDs are assigned after object
		//has been serialized).
		int GetObjectEntry(GenericObject* serializedObject);
		atomic_ptr<GenericObject> GetSerializable(int serialID) { return serials[serialID]; }

		std::vector<atomic_ptr<GenericObject> > serials;		//contains serialized objects
	};
}

#endif