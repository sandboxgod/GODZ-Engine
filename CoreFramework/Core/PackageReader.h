/* ===========================================================
	Package Reader

	Reads Godz package files.

	Created Apr 11, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_PACKAGE_READER_H_)
#define _PACKAGE_READER_H_

#include "Godz.h"
#include "GenericObject.h"
#include "GenericPackage.h"


namespace GODZ
{
	//forward decl
	class GenericPackage;
	class ResourceFactory;

	//Reads GODZ file packages (PackageFlag=PF_GDZ)
	class GODZ_API PackageReader
	{
	public:
		PackageReader(ResourceFactory* factory);

		//Reads the godz package at the file destination
		void ReadPackage(const char* filename);

		//Read package dependencies from the file and return a list
		void ReadPackageDependencies(const char* filename, PackageDependencies& dependencies);

		//Returns the GODZ object package
		GenericPackage* GetPackage();

	private:
		GenericPackage* package;
		ResourceFactory* m_factory;
	};
}

#endif