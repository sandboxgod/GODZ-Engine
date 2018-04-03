/* ===========================================================
	File Reader implementation

	Created Feb 26, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_FILEREADER_H_)
#define _FILEREADER_H_

#include "IReader.h"
#include "SerializedContentHandler.h"
#include "NameTable.h"
#include "atomic_ref.h"

namespace GODZ
{
	class ClassProperty;
	class ResourceFactory;

	class GODZ_API FileReader : public IReader
	{
	public:
		// [in] filename - file to open.
		FileReader(const char* filename, ResourceFactory* factory);
		~FileReader();

		void			Close();

		//Reads the godz file package creating objects based on serialized object data
		//encountered within the file.
		// [in] objectLoader - object is used for special circumstances whereas the loader
		// needs to determine how to load an object (for ex. a LevelNode can spawn WEntity
		// objects)
		GenericPackage* ReadPackage(IObjectLoader* objectLoader=NULL);

		//Read package dependencies from the file and return a list
		void		ReadPackageDependencies(PackageDependencies& depenecies);

		//IReader interface
		void			ReadBool(bool& b);
		size_t			ReadData(void* data, size_t sizeOfData, size_t primitives);
		void			ReadUnsignedInt(size_t& d);
		void			ReadLong(long& val);

		void			ReadFloat(float& f);
		void			ReadQuat4f(Quat4f& q);
		void			ReadWMatrix16f(WMatrix& m);
		void			ReadMatrix3(WMatrix3& m);
		void			ReadU8(u8& value);
		atomic_ptr<GenericObject>	ReadObject();

		bool			ReadProperty(ClassProperty* property, GenericObject* go);
		void			ReadVector(Vector3& v);
		void			ReadInt(int& i);
		rstring			ReadString();
		void			ReadUDWORD(udword& d);
		void			ReadU16(u16& value);
		void			ReadUInt32(UInt32& value);
		void			ReadBBox(WBox& bbox);
		void			ReadULONG(unsigned long& d);
		void			ReadGUID(GUID& uid);
		virtual ResourceObject* ReadResource();
		virtual bool ReadStruct(StructData& data);

	private:
		GenericPackage* FindPackage(UInt32 hash);

		PackageHeader mHeader;
		SerializedContent mContent;
		FILE *			stream;
		NameTable		tbl;
		IObjectLoader *	m_pObjectLoader;
		GDZArchive *	m_pAr;

		struct PropertyList
		{
			GenericObject* object;
			fpos_t m_nPos;
		};

		//object property loading is postphoned til all objects are loaded. This
		//way circular dependencies can be handled...
		std::vector<PropertyList> mObjectProperty; 

		atomic_ptr<GenericObject> ReadClass(int iObjEntry);

		GenericPackage* m_package;
		ResourceFactory* m_factory;

		std::vector<ObjectEntry> objectEntry;		//list of object entries within package
		std::vector<PackageEntry> packageEntry;		//list of packages

		rstring mFilename;


		//local package map, caches all the packages we've loaded so we can grab it quickly again
		typedef stdext::hash_map<HashCodeID, GenericPackage*> PackageMap;
		PackageMap mPackageMap;
	};
}

#endif