/* ===========================================================
	File Writer implementation

	Created Feb 24, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_FILEWRITER_H_)
#define _FILEWRITER_H_

#include "IWriter.h"
#include "NameTable.h"
#include "SerializedContentHandler.h"
#include "GenericPackage.h"
#include <hash_map>


namespace GODZ
{
	class ClassProperty;

	class GODZ_API FileWriter : public IWriter
	{
	public:
		// [in] filename - file to write data to.
		FileWriter(const char* filename);
		virtual ~FileWriter();

		void Close();
		bool HasStream() { return stream != NULL; }
		
		int FindPackage(GenericPackage* p);
		
		//Current file pointer position
		int GetSeekPos();

		//Writes a package to the stream.
		// [in] package - generic object package that will be serialized. All objects
		// that don't belong to the objects arg will be marked as an 'import' and won't
		// be serialized by this object.
		// [in] objects - contains the list of all objects that will be written to the
		// stream. This MUST include all object references and tagged properties.
		void WritePackage(GenericPackage* package, std::vector<GenericObject*>& objects);

		//////////////////////////////////////////////////////////////////////////////////
		//IWriter interface
		//////////////////////////////////////////////////////////////////////////////////

		void WriteBool(bool b);
		size_t WriteData(void* data, size_t sizeOfData, size_t primitives);
		void WriteUnsignedInt(size_t data);
		void WriteFloat(float v);
		void WriteQuat4f(const Quat4f& q);
		void WriteWMatrix16f(const WMatrix& m);
		void WriteMatrix3(const Matrix3& m);
		void WriteU8(u8& ch);
		virtual void WriteUInt32(UInt32 us);
		void WriteLong(long& val);

		//Writes object to the stream. If the object isn't a member of this writer's
		//internal object list, false is returned.
		void WriteObject(GenericObject* obj);

		void WriteProperty(ClassProperty* property, GenericObject* go);
		void WriteVector(const Vector3& v);
		void WriteInt(int i);
		void WriteString(const char* text);
		void WriteUDWORD(udword& v);
		void WriteU16(u16& us);
		void WriteBBox(WBox& bbox);
		void WriteULONG(unsigned long value);
		void WriteGUID(GUID& uid);
		virtual void WriteResource(ResourceObject* resource);
		virtual void WriteStruct(Struct* myStruct, void* data, size_t sizeOf);

		//special use; adds package dependency
		int AddPackage(GenericPackage* pkg);

	private:
		void ExportResource(ResourceObject* resource);
		
		bool WriteClass(GenericObject* serializedObject);
		void UpdateObjectInfo(GenericObject* obj);
		void SerializeObject(GenericObject* obj);

		PackageHeader mHeader;							//table of contents
		FILE* stream;
		NameTable tbl;
		GenericPackage* mPackage;
		std::vector<GenericPackage*> packages;			//package table
		GDZArchive* m_pArchive;
		std::vector<GenericObject*> externals;
		std::vector<GenericObject*> internalObjects;	//internal objects owned by another

		//stores a list of all the resource we've already written....
		typedef stdext::hash_map<HashCodeID, ResourceObject*> ResourceList;
		ResourceList		m_resourceList;

		std::vector<ObjectEntry> objectEntry;			//list of object entries within package
		std::vector<PackageEntry> packageEntry;			//list of packages

		SerializedContentHandler mContent;
	};
}

#endif