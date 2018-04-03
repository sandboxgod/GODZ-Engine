/* ===========================================================
	Reader interface

	Created Feb 26, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_IREADER_H_)
#define _IREADER_H_

#include "Godz.h"
#include "atomic_ref.h"
#include "GenericObject.h"
#include <CoreFramework/Math/WBox.h>
#include <CoreFramework/Math/WMatrix.h>
#include <CoreFramework/Math/Quaternion.h>
#include <CoreFramework/Reflection/StructData.h>

namespace GODZ
{
	class ClassProperty;
	class IObjectLoader;

	//Reader interface
	class GODZ_API IReader
	{
	public:

		//Closes the stream
		virtual void Close()=0;

		//Reads a boolean from the stream
		virtual void ReadBool(bool &b)=0;

		//Reads a binary chunk from the stream
		virtual size_t ReadData(void *data, size_t sizeOfData, size_t primitives)=0;

		//Reads an unsigned int from the stream
		virtual void ReadULONG(ulong &d)=0;

		virtual void ReadUDWORD(udword &d)=0;
		virtual void ReadU16(u16 &value)=0;
		virtual void ReadU8(u8 &value)=0;
		virtual void ReadLong(long &val)=0;
		virtual void ReadQuat4f(Quat4f& q) = 0;

		//Reads a float from the stream
		virtual void ReadFloat(float &v)=0;

		//Reads a godz package (PF_GDZ) from the stream. Each time a serialized
		//object is encountered, it's immediately instantiated. 
		// [in] objectLoader - if the reader isn't able to directly instantiate
		// an object, this object is used to load the object (for ex., WEntity
		// objects are spawned on a LevelNode so pass in the LevelNode).
		virtual GenericPackage* ReadPackage(IObjectLoader *objectLoader=NULL)=0;

		//Reads an integer from the stream
		virtual void ReadInt(int &i)=0;
		virtual void ReadUInt32(UInt32& u) = 0;

		virtual void ReadGUID(GUID& uid) = 0;

		//Reads an object from the stream
		virtual atomic_ptr<GenericObject>	ReadObject() = 0;

		//Reads a property from the source
		virtual bool ReadProperty(ClassProperty *property, GenericObject* obj)=0;

		//Reads a string from the stream
		virtual rstring ReadString()=0;

		//Reads a vector from the stream
		virtual void ReadVector(Vector3& v)=0;

		virtual void ReadBBox(WBox& bbox)=0;
		virtual void ReadWMatrix16f(WMatrix& m)=0;

		virtual ResourceObject* ReadResource()=0;

		virtual bool ReadStruct(StructData& data)=0;
	};
}

#endif