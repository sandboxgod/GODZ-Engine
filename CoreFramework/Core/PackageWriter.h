/* ===========================================================
	Package Writer

	Writes Godz package files. Note, dependency objects are
	automatically serialized (this includes aggregated objects
	such as objects encapsulated within a property or returned
	from a GetObjectReferences() query).

	Created Apr 10, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_PACKAGE_WRITER_H_)
#define _PACKAGE_WRITER_H_

#include "Godz.h"
#include "GenericObject.h"

namespace GODZ
{
	//Serializes a Generic Package to a file.
	class GODZ_API PackageWriter
	{
	public:
		PackageWriter();

		//Serializes the package to the file destination
		bool SavePackage(GenericPackage *package, const char *filename);

	private:
		GenericPackage *package;
		std::vector<GenericObject*> generics;

		bool ContainsObject(GenericObject *obj);
	};
}

#endif