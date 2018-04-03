/* ===========================================================
	File Writer interface

	Created Feb 24, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_IWRITER_H_)
#define _IWRITER_H_

#include "Godz.h"
#include <CoreFramework/Math/WBox.h>
#include <CoreFramework/Math/Quaternion.h>

namespace GODZ
{
	//forward declar
	class ClassProperty;
	class GenericPackage;
	class ResourceObject;
	class Struct;

	//Writer interface
	class GODZ_API IWriter
	{
	public:
		virtual ~IWriter() {}

		//Closes the stream
		virtual void Close()=0;

		//Writes a bool primitive
		virtual void WriteBool(bool b)=0;

		//Writes a binary chunk to the stream
		virtual size_t WriteData(void* data, size_t sizeOfData, size_t primitives)=0;
		virtual void WriteStruct(Struct* myStruct, void* data, size_t sizeOf)=0;

		virtual void WriteULONG(ulong d)=0;
		virtual void WriteUDWORD(udword& v)=0;
		virtual void WriteUInt32(UInt32 us)=0;
		virtual void WriteU16(UInt16& us)=0;
		virtual void WriteU8(u8& ch)=0;
		virtual void WriteLong(long& val)=0;
		virtual void WriteGUID(GUID& uid) = 0;

		//Writes a float to the stream
		virtual void WriteFloat(float v)=0;

		// Creates a godz package file (Package flag PF_GDZ) containing the serializable 
		// data of the object list (objects).
		// [in] package - package of objects being serialized (objects exported from this package). All
		// objects that are referenced by the list that do not belong to this package are 
		// marked as 'imports' because their data is relied upon an object within this package.
		// [in] objects - list of objects that need to be serialized.
		virtual void WritePackage(GenericPackage* package,std::vector<GenericObject*>& objects)=0;

		//Writes the integer to the stream
		virtual void WriteInt(int i)=0;

		//Writes an object to the stream.
		virtual void WriteObject(GenericObject* obj)=0;

		//Writes the property to the stream
		virtual void WriteProperty(ClassProperty* property, GenericObject* obj)=0;

		//Writes the string to the stream
		virtual void WriteString(const char* text)=0;

		//Writes the vector to the stream
		virtual void WriteVector(const Vector3& v)=0;

		virtual void WriteBBox(WBox& bbox)=0;
		virtual void WriteWMatrix16f(const WMatrix16f& m)=0;

		virtual void WriteResource(ResourceObject* resource)=0;
	};
}

#endif