
#include "FileReader.h"
#include "GenericObject.h"
#include "ResourceFactory.h"
#include "GenericObjData.h"
#include "StringTokenizer.h"
#include "debug.h"
#include <CoreFramework/Reflection/ClassProperty.h>
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/PropertyFactory.h>
#include <CoreFramework/Reflection/Struct.h>
#include <CoreFramework/Reflection/StructProperty.h>

namespace GODZ
{



FileReader::FileReader(const char* filename, ResourceFactory* factory)
: stream(NULL)
, m_pObjectLoader(NULL)
, m_package(NULL)
, m_factory(factory)
, m_pAr(NULL)
{
	fileOpen(&stream, filename, FileOpenMode_ReadBinary);
	if( stream == NULL )
	{	
		Log("Cannot open file %s\n", filename);
	}
}

FileReader::~FileReader()
{
	Close();
}

void FileReader::Close()
{
	if (stream != NULL)
	{
		fclose(stream);
		stream = NULL;
	}
}

void FileReader::ReadBool(bool& b)
{	
	fread(&b, sizeof(bool), 1, stream); 
}

atomic_ptr<GenericObject> FileReader::ReadClass(int iObjEntry)
{
	fseek(stream, objectEntry[iObjEntry].objOffset, SEEK_SET);

	//Load the package this class belongs to....
	int iPackageIndex = objectEntry[iObjEntry].iPkgIndex;
	if (!packageEntry[iPackageIndex].bExport)
	{
		atomic_ptr<GenericObject> obj_ptr;

		//get the preexisting object from the package and return
		HashCodeID hash = packageEntry[iPackageIndex].mHash;
		GenericPackage* p = GenericObjData::m_packageList.FindPackageByName(hash);
		if ( p != NULL)
		{
			obj_ptr = p->FindObjectRef( objectEntry[iObjEntry].mClassHash );
		}

		return obj_ptr;
	}

	GenericClass* metaClass = StaticLoadObject( objectEntry[iObjEntry].mClassHash );
	debug_assert(metaClass!=NULL, GetString("Could not find [hash = %d]. Make sure the DLL is loaded and it has called GenericClass::GetClass() to register the object.", objectEntry[iObjEntry].mClassHash )); //missing class instance
	
	atomic_ptr<GenericObject> object( metaClass->GetInstance() );

	debug_assert(object!=NULL,"Could not create object data.");

	// TODO ~ Update this code to invoke the new way of accessing GUIDs
	GUID id = GUID_NULL;

	//Read the Object GUID
	if ( mHeader.iFileVersion >= 1010 )
	{
		ReadData( &id, sizeof(GUID), 1);
	}

	m_package->AddObject(object, id );
	
	PropertyList pl;
	fgetpos(stream,&pl.m_nPos);
	pl.object=object;
	mObjectProperty.push_back(pl);


	return object;
}

size_t FileReader::ReadData(void* data, size_t sizeOfData, size_t primitives)
{
	godzassert(data!=NULL); //make sure you allocate the buffer	
	return fread(data, sizeOfData, primitives, stream); 
}


void FileReader::ReadFloat(float& f)
{	
	fread(&f, sizeof(float), 1, stream); 
}

void FileReader::ReadLong(long& f)
{	
	fread(&f, sizeof(long), 1, stream); 
}

void FileReader::ReadUnsignedInt(size_t& f)
{	
	fread(&f, sizeof(size_t), 1, stream); 
}

void FileReader::ReadU8(u8& value)
{
	fread(&value, sizeof(u8), 1, stream); 
}

void FileReader::ReadQuat4f(Quat4f& q)
{
	fread(&q, sizeof(float), 4, stream); 
}

void FileReader::ReadWMatrix16f(WMatrix16f& m)
{
	fread(&m, sizeof(float), 16, stream); 
}

void FileReader::ReadMatrix3(Matrix3& m)
{
	fread(&m, sizeof(float), 9, stream); 
}

void FileReader::ReadPackageDependencies(PackageDependencies& depenecies)
{
	//Reads package dependencies from the file and return a list
	fread(&mHeader, sizeof(PackageHeader), 1, stream);

	tbl.Clear();
	if (mHeader.names>0)
	{
		//yes i know the file ptr is probably already here but going to seek anyways
		//just in case file format changes a bit later
		fseek(stream, mHeader.nameTblOffset, SEEK_SET);
		tbl.LoadContent(this, mHeader.names);
	}

	PackageEntry* tempPkg = new PackageEntry[mHeader.packages];

	fseek(stream, mHeader.pkgTblOffset, SEEK_SET);

	//read all package entries from the stream...
	fread(&tempPkg[0], sizeof(PackageEntry), mHeader.packages, stream);

	for(int i=0; i<mHeader.packages;i++)
	{
		packageEntry.push_back(tempPkg[i]);
	}
	delete[] tempPkg;


	for(int i=0;i<mHeader.packages;i++)
	{
		if (!packageEntry[i].bExport)
		{
			depenecies.Add( packageEntry[i].mHash );
		}
	}
}

GenericPackage* FileReader::ReadPackage(IObjectLoader* objectLoader)
{
	if (!stream)
		return 0;

	m_pObjectLoader=objectLoader;

	fread(&mHeader, sizeof(PackageHeader), 1, stream);

	//get package name
	mFilename = ReadString();

	GDZArchive ar(this);
	m_pAr=&ar;
	ar.SetFileVersion(mHeader.iFileVersion);
	//godzassert(mHeader.iFileVersion <= 1009); // temp; check file version. cant read upgraded files yet

	tbl.Clear();
	bool bConfirmFormat = (mHeader.magic==MAGIC_NUMBER);
	debug_assert(bConfirmFormat,"This is not a GODZ file.");
	debug_assert(mHeader.packages>0, "Corrupt GODZ Package. Does not export any Packages!");

	if (mHeader.names>0)
	{
		//yes i know the file ptr is probably already here but going to seek anyways
		//just in case file format changes a bit later
		fseek(stream, mHeader.nameTblOffset, SEEK_SET);
		tbl.LoadContent(this, mHeader.names);
	}

	//Do list allocations
	objectEntry.reserve(mHeader.objects);
	packageEntry.reserve(mHeader.packages);

	PackageEntry* tempPkg = new PackageEntry[mHeader.packages];
	ObjectEntry* tempObjectPkg = new ObjectEntry[mHeader.objects];

	//TODO: need to stop seeking from 0! 
	if (mHeader.objects>0)
	{
		fseek(stream, mHeader.objTblOffset, SEEK_SET);
		fread(&tempObjectPkg[0], sizeof(ObjectEntry), mHeader.objects, stream);
	}

	for(size_t i = 0; i <mHeader.objects;i++)
	{
		objectEntry.push_back(tempObjectPkg[i]);
	}

	delete[] tempObjectPkg;

	fseek(stream, mHeader.pkgTblOffset, SEEK_SET);

	//read all package entries from the stream...
	fread(&tempPkg[0], sizeof(PackageEntry), mHeader.packages, stream);
	
	int packageIndex = -1;
	for(int i = 0; i <mHeader.packages;i++)
	{
		packageEntry.push_back(tempPkg[i]);

		if (tempPkg[i].bExport)
		{
			packageIndex = i;
		}
	}
	delete[] tempPkg;


	godzassert (m_package == NULL);
	
	m_package = new GenericPackage(mFilename, packageEntry[packageIndex].mHash,packageEntry[packageIndex].flag);

	m_package->ReserveObjects( mHeader.objects );

	for(unsigned int i=0;i<mHeader.objects;i++)
	{
		atomic_ptr<GenericObject> obj_ptr = ReadClass(i);
		if (obj_ptr!=NULL)
		{
			mContent.serials.push_back(obj_ptr);
		}
	}

	//Load Resources! This needs to be done before loading the objects....
	fseek(stream, mHeader.resourceOffset, SEEK_SET);
	for (int i = 0; i < mHeader.numResources; i++)
	{
		ulong resourceType;
		ReadULONG(resourceType); //todo: should be INT!

		rstring filename = ReadString();

		ulong resourceHash;
		ReadULONG(resourceHash);

		ResourceObject::ResourceFlag rflag = (ResourceObject::ResourceFlag)resourceType;
		GenericResource ref = m_factory->GetNewResource(rflag, resourceHash, filename);
		ref->Serialize(*m_pAr);
		ref->SetPackage(m_package);
	}

	//Load all the object references now that all the objects have been serialized..
	std::vector<PropertyList>::iterator propIter;
	for(propIter=mObjectProperty.begin();propIter!=mObjectProperty.end();propIter++)
	{
		PropertyList& propObj = (*propIter);
		GenericObject* object = propObj.object;
		fseek(stream, propObj.m_nPos, SEEK_SET);

		//load object properties
		udword count=0; //# of tagged properties
		ReadUDWORD(count);

		GenericClass* gc = object->GetRuntimeClass();

		for (size_t i=0;i<count;i++)
		{
			//now get current file ptr; we bookmark the file ptr before we begin accessing property data
			fpos_t pos;
			int result = fgetpos(stream, &pos);
			godzassert(result == 0);

			//get location of next property incase we need to advance file ptr forward
			udword offset_of_next_property;
			ReadUDWORD(offset_of_next_property);

			HashCodeID nameHash;
			ReadUInt32(nameHash);


			//Find the property
			bool hasRead = false;

			ClassProperty* property = gc->GetProperty(nameHash);
			if (property!=NULL)
			{
				hasRead = ReadProperty(property, object);
			}

			if (!hasRead)
			{
				Log("Object definition of type (%s) is missing property (%lu) from package %s.\n", object->GetRuntimeClass()->GetName(), nameHash, m_package->GetName());
				
				//advance to next property if theres another (or the next part)
				fpos_t temp = (fpos_t)offset_of_next_property;
				result = fsetpos(stream, &temp);
				godzassert(result == 0);
			}
		}
		
		object->Serialize(ar);
		//_asm nop;
	} //loop

	//---------------------------------------------------------
	//Wrap Up!
	//---------------------------------------------------------

	//clear out the ptr to the archive
	m_pAr = NULL;

	for(int i=0;i<mHeader.packages;i++)
	{
		if (!packageEntry[i].bExport)
		{
			m_package->AddDependency( packageEntry[i].mHash );
		}
	}

	//Get the Package that was exported from the file.
	return m_package;
}

void FileReader::ReadInt(int &i)
{
	fread(&i, sizeof(int), 1, stream);
}

void FileReader::ReadGUID(GUID& uid)
{
	fread(&uid, sizeof(GUID), 1, stream);
}

atomic_ptr<GenericObject> FileReader::ReadObject()
{
	//get object entry ID
	int serialID;
	ReadInt(serialID);

	if (serialID == -1)
	{
		atomic_ptr<GenericObject> empty_ptr;
		return empty_ptr;
	}

	if (packageEntry[ objectEntry[ serialID ].iPkgIndex ].bExport)
	{
		size_t size = mContent.serials.size();
		godzassert(serialID < static_cast<int>(size));		
		atomic_ptr<GenericObject> ptr = mContent.GetSerializable(serialID);
		return ptr;
	}

	GUID id;
	ReadData(&id, sizeof(GUID), 1);

	HashCodeID phash = packageEntry[ objectEntry[ serialID ].iPkgIndex ].mHash;
	GenericPackage* p = FindPackage(phash);
	if (p != NULL)
	{
		return p->FindObjectRefByGUID(id);
	}

	godzassert(p != NULL);
	atomic_ptr<GenericObject> empty_ptr;
	return empty_ptr;
}

bool FileReader::ReadProperty(ClassProperty* property, GenericObject* go)
{
	EPropertyType type;
	fread(&type, sizeof(EPropertyType), 1, stream);

	if (type!=property->GetPropertyType())
	{
		return false;
	}

	switch(type)
	{
	case(PT_FLOAT):
		{
			float f;
			ReadFloat(f);
			property->SetFloat(go, f);
		}
		break;

	case(PT_COLOR):
		{
			Color4f color;

			ReadFloat(color.r);
			ReadFloat(color.g);
			ReadFloat(color.b);
			ReadFloat(color.a);

			property->SetColor(go, color);

		}
		break;

	case(PT_INT):
		{
			int i;
			ReadInt(i);
			property->SetInt(go, i);
		}
		break;

	case(PT_UINT32):
		{
			UInt32 i;
			ReadUInt32(i);
			property->SetUInt32(go, i);
		}
		break;

	case(PT_HASHCODE):
		{
			UInt32 i;
			ReadUInt32(i);
			property->SetHashCode(go, i);
		}
		break;

	case(PT_HASHSTRING):
		{
			UInt32 i;
			ReadUInt32(i);
			property->SetHashString(go, i);
		}
		break;

	case(PT_BOOL):
		{
			bool flag;
			ReadBool(flag);
			property->SetBool(go, flag);
		}
		break;
	case(PT_STRING):
		{
			rstring text;
			text=ReadString();
			property->SetString(go, text);
		}
		break;
	case(PT_CHARARRAY):
		{
			rstring text;
			text=ReadString();
			property->SetString(go, text);
		}
		break;
	case(PT_VECTOR):
		{
			Vector3 v;
			ReadVector(v);
			property->SetVector(go, v);
		}
		break;

	case(PT_QUATERNION):
		{
			Quat4f q;
			ReadQuat4f(q);
			property->SetQuaternion(go, q);
		}
		break;

	case(PT_EULER):
		{
			Vector3 v;
			ReadVector(v);
			EulerAngle rot(v.x,v.y,v.z);
			property->SetRotation(go, rot);
		}
		break;
	case PT_MATRIX:
		{
			WMatrix m;
			ReadWMatrix16f(m);
			property->SetMatrix(go, m);
		}
		break;
	case PT_MATRIX3:
		{
			Matrix3 m;
			ReadMatrix3(m);
			property->SetMatrix3(go, m);
		}
		break;
	case(PT_BOX):
		{
			Vector3 bmin,bmax;
			WBox b;
			ReadVector(bmin);
			ReadVector(bmax);
			b.SetMinMax(bmin, bmax);

			property->SetBox(go, b);
		}
		break;
	case(PT_OBJECT):
		{
			//get object reference
			atomic_ptr<GenericObject> objectPtr = ReadObject();
			if (objectPtr != NULL)
			{
				property->setObject(go, objectPtr);
			}
		}
		break;
	case(PT_ARRAY):
		{
			size_t size;
			ReadUDWORD(size);

			int propertyType;
			ReadInt(propertyType);

			//element property type does not match...
			bool doesMatch = propertyType == property->GetArrayElementType();
			godzassert(doesMatch);

			if (doesMatch)
			{
				for (size_t i = 0; i < size; i++)
				{
					EPropertyType type = (EPropertyType)propertyType;
					GenericReference<PropertyValue> value = PropertyFactory::create(type);
					godzassert(value != NULL); //unknown type???
					value->load(this);
					property->AddValue(go, value);
				}
			}
			else
			{
				return false;
			}
		}
		break;
	case (PT_MAP):
		{
			size_t size;
			ReadUDWORD(size);

			for (size_t i = 0; i < size; i++)
			{
				int keyPropertyType;
				ReadInt(keyPropertyType);

				//verify types match
				if (keyPropertyType != property->GetKeyElementType(go))
				{
					return false;
				}

				GenericReference<PropertyValue> key = PropertyFactory::create((EPropertyType)keyPropertyType);
				godzassert(key != NULL); //unknown type???
				key->load(this);

				int valuePropertyType;
				ReadInt(valuePropertyType);

				//verify types match
				if (valuePropertyType != property->GetValueElementType(go))
				{
					return false;
				}

				GenericReference<PropertyValue> v = PropertyFactory::create((EPropertyType)valuePropertyType);
				godzassert(v != NULL); //unknown type???
				v->load(this);

				property->AddElement(go, key, v);
			}
		}
		break;
	case(PT_GUID):
		{
			GUID id;
			ReadData(&id, sizeof(GUID), 1);
			property->SetGUID(go, id);
		}
		break;
	case(PT_RESOURCE):
		{
			ResourceObject* pRes = ReadResource();

			if (pRes != NULL)
			{
				property->SetResource(go, pRes);
			}
		}
		break;
	default:
		//file reader not implemented for property type
		debug_assert(0,"Property type is unknown.");
		break;
	}

	return true;
}

GenericPackage* FileReader::FindPackage(UInt32 packageHash)
{
	if (m_package->GetPackageID() == packageHash)
	{
		return m_package;
	}

	GenericPackage* pack = NULL;

	PackageMap::iterator iter;
	iter = mPackageMap.find(packageHash);


	if (iter != mPackageMap.end())
	{
		return iter->second;
	}
	else
	{
		pack = GenericObjData::m_packageList.FindPackageByName(packageHash);
		mPackageMap[packageHash] = pack;
	}

	return pack;
}

ResourceObject* FileReader::ReadResource()
{
	bool hasResource;
	ReadBool(hasResource);
	if (hasResource)
	{
		ulong flags;
		ReadULONG(flags);

		//get the crc32 value for this resource...
		HashCodeID hashCode;
		ReadUInt32(hashCode);

		HashCodeID packageHash;
		ReadUInt32(packageHash);

		GenericPackage* pack = FindPackage(packageHash);
		
		if (pack != NULL)
		{
			return pack->findResource(hashCode);
		}
	}

	return NULL;
}

rstring FileReader::ReadString()
{
	//first get the length of the string
	int len=0;
	ReadInt(len);

	//create the char buffer
	if (len > 0)
	{
		char* data = static_cast<char*>(malloc(len+1));
		fread(data, sizeof(char), len, stream);	
		data[len]='\0';

		rstring text = data;
		free(data);
		return text;
	}

	return "";
}

void FileReader::ReadVector(Vector3& v)
{
	fread(&v.x, sizeof(Vector3), 1, stream);
}

void FileReader::ReadUDWORD(udword& d)
{
	fread(&d, sizeof(udword), 1, stream);
}

void FileReader::ReadU16(u16& value)
{
	fread(&value, sizeof(u16), 1, stream);
}

void FileReader::ReadUInt32(UInt32& value)
{
	fread(&value, sizeof(UInt32), 1, stream);
}

void FileReader::ReadBBox(WBox& bbox)
{
	Vector3 min,max;
	fread(&min.x, sizeof(float), 3, stream);
	fread(&max.x, sizeof(float), 3, stream);

	bbox.SetMinMax(min, max);
}

void FileReader::ReadULONG(unsigned long& value)
{
	fread(&value, sizeof(unsigned long), 1, stream);
}

bool FileReader::ReadStruct(StructData& data)
{
	//todo: read struct, allocate memory, etc
	UInt32 structType;
	ReadUInt32(structType);
	data.m_struct = Struct::FindStruct(structType);

	if (data.m_struct == NULL)
		return false;

	UInt32 numProperties;
	ReadUInt32(numProperties);

	ReadUInt32(data.m_size);
	data.m_data = new UInt8[data.m_size];

	for(UInt32 i = 0; i < numProperties; i++)
	{
		UInt32 propertyName;
		ReadUInt32(propertyName);

		StructProperty* s = data.m_struct->GetProperty(propertyName);
		if (s == NULL)
		{
			//property no longer valid
			return false;
		}

		int propertyType;
		ReadInt(propertyType);

		if (propertyType != s->GetType(data.m_struct))
		{
			return false;
		}

		GenericReference<PropertyValue> v = PropertyFactory::create((EPropertyType)propertyType);
		v->load(this);
	}

	return true;
}


} //namespace GODZ