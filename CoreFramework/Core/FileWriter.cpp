
#include "FileWriter.h"
#include "GenericObject.h"
#include "GenericPackage.h"
#include "ResourceObject.h"
#include "debug.h"
#include <CoreFramework/Reflection/ClassProperty.h>
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Reflection/Struct.h>
#include <CoreFramework/Reflection/StructProperty.h>
#include <stdio.h>



namespace GODZ
{

class FileWriterMapCallback : public MapCallback
{
public:
	FileWriterMapCallback(FileWriter* writer)
		: mWriter(writer)
	{
	}

	virtual void analyzeMapPair(GenericReference<PropertyValue> key, GenericReference<PropertyValue> value)
	{
		EPropertyType keyType = key->getType();
		mWriter->WriteInt(keyType);
		key->save(mWriter);

		EPropertyType valueType = value->getType();
		mWriter->WriteInt(valueType);
		value->save(mWriter);
	}

private:
	FileWriter* mWriter;
};


class ObjPackageMapCallback : public MapCallback
{
public:
	ObjPackageMapCallback(FileWriter* writer)
		: mWriter(writer)
	{
	}

	virtual void analyzeMapPair(GenericReference<PropertyValue> key, GenericReference<PropertyValue> value)
	{
		if( value->getType() == PT_OBJECT )
		{
			PropertyValue* temp = value;
			PropertyObject* po = (PropertyObject*)temp;
			GenericObject* objectElement = po->getObject();

			mWriter->AddPackage( objectElement->GetPackage() );

			//write out class info too
			GenericClass* p = objectElement->GetRuntimeClass();
			while(p != NULL)
			{
				mWriter->AddPackage( p->GetPackage() );
				p = p->GetSuperClass();
			}
		}
	}

private:
	FileWriter* mWriter;
};
//////////////////////////////////////////////////////////////////////////

FileWriter::FileWriter(const char* filename)
: stream(NULL)
, m_pArchive(NULL)
{
	fileOpen(&stream, filename, FileOpenMode_WriteBinary);
	if( stream == NULL )
	{
		debug_assert(0, GetString("Cannot open file %s", filename));
	}
}

FileWriter::~FileWriter()
{
	if (stream != NULL)
	{
		fclose(stream);
		stream = NULL;
	}

	if (m_pArchive != NULL)
	{
		delete m_pArchive;
		m_pArchive=NULL;
	}
}

int FileWriter::AddPackage(GenericPackage* pkg)
{
	//Assume object is being saved internally...
	if ( pkg == NULL )
		pkg = (mPackage);

	int index=FindPackage(pkg);
	if (index!=-1)
	{
		return index;
	}

	packages.push_back(pkg);
	index = (int)packages.size() - 1;

	PackageEntry entry;
	entry.flag=pkg->GetPackageFlag();
	entry.mHash=pkg->GetPackageID();

	//Indicate if this package is internal or external
	if (pkg == mPackage || pkg->GetPackageID() == mPackage->GetPackageID())
	{
		entry.bExport=true;
	}
	else
	{
		entry.bExport=false;
	}

	packageEntry.push_back(entry);

	return index;
}

void FileWriter::Close()
{
	//update the file header...
	fseek(stream, 0, SEEK_SET);
	fwrite(&mHeader, sizeof(PackageHeader), 1, stream);

	//update the object entry table....
	fseek(stream, mHeader.objTblOffset, SEEK_SET);
	fwrite(&objectEntry[0], sizeof(ObjectEntry), mHeader.objects, stream);
	fclose(stream);

	if (m_pArchive != NULL)
	{
		delete m_pArchive;
		m_pArchive=NULL;
	}
}

int FileWriter::GetSeekPos()
{
	fpos_t off;
	fgetpos(stream,&off);
	return (int)off;
}

int FileWriter::FindPackage(GenericPackage* p)
{
	int index=0;
	std::vector<GenericPackage*>::iterator pkgIter;
	for(pkgIter=packages.begin();pkgIter!=packages.end();pkgIter++)
	{
		if ( (*pkgIter)==p || (*pkgIter)->GetPackageID() == p->GetPackageID())
			return index;

		index++;
	}

	return -1;
}

void FileWriter::WriteBool(bool b)
{
	fwrite(&b, sizeof(bool), 1, stream); 
}

bool FileWriter::WriteClass(GenericObject *sObj)
{
	int iObjIndex = mContent.GetObjectEntry(sObj);
	godzassert(iObjIndex!=-1);

	//update the matching object entry
	objectEntry[iObjIndex].objOffset=GetSeekPos();



	GenericClass* gc = sObj->GetRuntimeClass();
	godzassert(gc!=NULL);

	objectEntry[iObjIndex].mClassHash = gc->GetHashCode();

	//Check to see if this object will be exported.
	int iPkgIndex=objectEntry[iObjIndex].iPkgIndex;
	if (!packageEntry[iPkgIndex].bExport)
	{
		return true;
	}

	//Save the GUID for this object
	GUID objectID = mPackage->GetGUIDForObject(sObj);
	WriteData(&objectID, sizeof(GUID), 1);

	std::vector<ClassProperty*> list;
	gc->GetProperties(list);

	//count # of valid properties
	UInt32 numProps = 0; //static_cast<udword>(list.size());

	for(UInt32 i = 0; i < list.size(); i++)
	{
		if (!list[i]->IsObsolete())
		{
			numProps++;
		}
	}

	WriteUDWORD(numProps);

	//Log("save %s %d\n",className, numProps);
	
	return true;
}

size_t FileWriter::WriteData(void *data, size_t sizeOfData, size_t primitives)
{
	return fwrite(data, sizeOfData, primitives, stream); 
}

void FileWriter::WriteLong(long &d)
{
	fwrite(&d, sizeof(long), 1, stream); 
}

void FileWriter::WriteFloat(float f)
{
	fwrite(&f, sizeof(float), 1, stream); 
}

void FileWriter::WriteQuat4f(const Quat4f &q)
{
	fwrite(&q.q.x, sizeof(float), 4, stream); 
}

void FileWriter::WriteWMatrix16f(const WMatrix16f& m)
{
	fwrite(&m, sizeof(float), 16, stream); 
}

void FileWriter::WriteMatrix3(const Matrix3& m)
{
	fwrite(&m, sizeof(float), 9, stream); 
}

void FileWriter::UpdateObjectInfo(GenericObject* go)
{
	godzassert(go != NULL);
	debug_assert(go->IsAObject(), "Only dynamic objects can be serialized!");

	ObjectEntry entry;

	GenericClass* gc = go->GetRuntimeClass();

	entry.mClassHash = gc->GetHashCode();

	GenericPackage* pkg = go->GetPackage();

	//Assume this object is internal.
	if(pkg == NULL)
	{
		pkg = mPackage;
	}

	int index = AddPackage(pkg);

	//Add package for parent classes too.... This way all dependencies are accounted for....
	GenericClass* p = gc;
	while(p != NULL)
	{
		AddPackage(p->GetPackage());
		p = p->GetSuperClass();
	}

	//get the class package
	entry.iPkgIndex=index;
		
	if (packageEntry[index].bExport)
	{
		//Add property names to the name table...
		std::vector<ClassProperty*> list;
		gc->GetProperties(list);
		size_t num = list.size();
		for(size_t i=0;i<num;i++)
		{
			ClassProperty* gp = list[i];

			if (gp != NULL)
			{
				//if this a resource, add it's package to the dependency list....
				if (gp->GetPropertyType() == PT_RESOURCE)
				{
					ResourceObject* resource = gp->GetResource(go);
					if (resource != NULL)
					{
						AddPackage( resource->GetPackage() );
					}
				}

				//ditto, for objects
				else if (gp->GetPropertyType() == PT_OBJECT)
				{
					GenericObject* oo = gp->getObject(go);
					if (oo == NULL)
						continue;

					GenericPackage* op = oo->GetPackage();
					AddPackage( op );

					//write out class info too
					GenericClass* p = gc;
					while(p != NULL)
					{
						AddPackage(p->GetPackage());
						p = p->GetSuperClass();
					}
				}
				else if (gp->GetPropertyType() == PT_ARRAY)
				{
					//write object dependencies to the graph
					if (gp->GetArrayElementType() == PT_OBJECT)
					{
						size_t size = gp->GetSize(go);
						for(size_t i=0;i<size;i++)
						{
							GenericReference<PropertyValue> value = gp->GetValueAt(go, i);
							PropertyValue* temp = value;
							PropertyObject* po = (PropertyObject*)temp;
							GenericObject* objectElement = po->getObject();

							AddPackage(objectElement->GetPackage());

							//write out class info too
							GenericClass* p = objectElement->GetRuntimeClass();
							while(p != NULL)
							{
								AddPackage(p->GetPackage());
								p = p->GetSuperClass();
							}
						}
					}
				}
				else if (gp->GetPropertyType() == PT_MAP)
				{
					//write object dependencies to the graph
					//if (gp->GetValueElementType() == PT_OBJECT)
					{
						ObjPackageMapCallback mapIter(this);
						gp->IteratePairs(go, &mapIter);
					}
				}
			}
		}
	}

	objectEntry.push_back(entry);
}

void FileWriter::SerializeObject(GenericObject* obj)
{
	//write out the class header
	WriteClass(obj);

	//if object lacks a package we assume it's internal!
	GenericPackage* ooPack = obj->GetPackage();
	if ( ooPack == mPackage || ooPack == NULL )
	{
		fpos_t pos;

		//saves all serializable properties
		GenericClass* gc = obj->GetRuntimeClass();
		std::vector<ClassProperty*> list;
		gc->GetProperties(list);

		size_t num = list.size();
		for(size_t i = 0; i < num; i++)
		{
			int result = 0;

			ClassProperty* gp = list[i];
			if (gp != NULL && !gp->IsObsolete())
			{
				//save file ptr position
				result = fgetpos(stream, &pos);
				godzassert(result == 0);

				//store 0 for now; later we compute file offset of next property
				udword temp = 0;
				WriteUDWORD(temp);

				WriteProperty(gp, obj);

				//now get current file ptr position
				fpos_t pos2;
				result = fgetpos(stream, &pos2);
				godzassert(result == 0);

				//go back to original position & output the offset where next property is stored
				result = fsetpos(stream, &pos);
				godzassert(result == 0);

				temp = (udword)pos2;
				WriteUDWORD(temp);

				//now reset file ptr
				result = fsetpos(stream, &pos2);
				godzassert(result == 0);
			}
		}

		//allow the object this opportunity to save custom content
		obj->Serialize(*m_pArchive);
	}
}

void FileWriter::WritePackage(GenericPackage* package, std::vector<GenericObject*>& objects)
{
	m_pArchive = new GDZArchive(this);
	mPackage=package;
	mContent.serials=objects; //store copy of object list

	ZeroMemory(&mHeader, sizeof(PackageHeader));
	mHeader.magic=MAGIC_NUMBER;
	mHeader.iFileVersion=GODZ_SAVE_VERSION;
	
	fwrite(&mHeader, sizeof(PackageHeader), 1, stream);

	//write out package name
	WriteString(package->GetName());

	//go through the objects array and build the name and import table
	std::vector<GenericObject*>::iterator objIter;
	for(objIter=objects.begin();objIter!=objects.end();objIter++)
	{
		GenericObject* go = (*objIter);
		UpdateObjectInfo(go);
	}

	//serialize the root objects...
	for(objIter=objects.begin();objIter!=objects.end();objIter++)
	{
		GenericObject* obj = (*objIter);
		SerializeObject(obj);
	}

	//Now serialize the annoymous objects that we didn't know about that was discovered
	//when we introspected the objects being saved....
	std::vector<GenericObject*> objList;

	do
	{
		//keep checking internalObjects to see if new objects were submitted
		objList = internalObjects;
		internalObjects.clear();

		for ( objIter=objList.begin();objIter!=objList.end();objIter++ )
		{
			GenericObject* obj = (*objIter);
			SerializeObject(obj);
		}
	} while( internalObjects.size() > 0 ); //keep looping til no more objs are submitted

	//serialize external refs
	for(objIter=externals.begin();objIter!=externals.end();objIter++)
	{
		GenericObject* obj = (*objIter);
		WriteClass(obj);
	}

	//Write the Name Table
	mHeader.nameTblOffset = GetSeekPos();
	mHeader.names = static_cast<int>(tbl.GetNumStrings());
	tbl.SaveContent(this);

	//Write the Object Table
	mHeader.objTblOffset = GetSeekPos();

	//write all object entries to the stream...
	size_t objectCount = objectEntry.size();
	fwrite(&objectEntry[0], sizeof(ObjectEntry), objectCount, stream);

	//Write the Package Table
	mHeader.packages = (int)packages.size();
	mHeader.pkgTblOffset = GetSeekPos();

	//write all package entries into the stream...
	fwrite(&packageEntry[0], sizeof(PackageEntry), packages.size(), stream);

	//Write out all the Resources this package owns.....
	GenericPackage::ResourceList::iterator begin;
	GenericPackage::ResourceList::iterator end;
	GenericPackage::ResourceList::iterator iter;

	mPackage->GetResourceIterators(begin, end);

	mHeader.resourceOffset = GetSeekPos();

	int numresources = 0;
	for (iter = begin; iter != end; iter++)
	{
		ResourceObject* res = iter->second;
		godzassert(res != NULL);

		ExportResource(res);
		numresources++;
	}

	mHeader.numResources = numresources;

	//--------------------------------------------------------------------
	//LAST STEP: update the file header
	//--------------------------------------------------------------------

	fseek(stream, 0, SEEK_SET);

	mHeader.objects=objectCount;
	fwrite(&mHeader, sizeof(PackageHeader), 1, stream);

	if (m_pArchive != NULL)
	{
		delete m_pArchive;
		m_pArchive=NULL;
	}
}

void FileWriter::WriteInt(int i)
{
	fwrite(&i, sizeof(int), 1, stream);
}

void FileWriter::WriteU8(u8 &ch)
{
	fwrite(&ch, sizeof(unsigned char), 1, stream);
}

void FileWriter::WriteObject(GenericObject* obj)
{
	int index = mContent.GetObjectEntry(obj);
	bool isExternal = false;

	//allow archives to dynamically add objects to the serials list
	if (index == -1)
	{
		//This is an external object? Confirm...
		mContent.serials.push_back(obj);

		index = mContent.GetObjectEntry(obj);

		//add this object to the serializable table..
		UpdateObjectInfo(obj);

		//Determine if the object is really external
		GenericPackage* gp = obj->GetPackage();
		if( gp != NULL && gp != mPackage )
		{
			externals.push_back(obj);
			isExternal = true;
		}
		else
		{
			internalObjects.push_back(obj);
		}
	}

	//write out object reference (index within the ObjectEntry array)
	WriteInt(index);

	if (isExternal)
	{
		GUID objectID = GUID_NULL;
		bool hasID = obj->GetGUIDFromPackage(objectID);
		godzassert(hasID); // External objects must have a unique id we can reference it by

		WriteData(&objectID, sizeof(GUID), 1);
	}
}

void FileWriter::WriteProperty(ClassProperty* property, GenericObject* go)
{
	HashCodeID hash = property->GetHashCode();
	WriteUInt32(hash);

	//writing out the property type should let us know how many bytes this property
	//used up- this way if a property gets dropped we can still load the file later
	EPropertyType type = property->GetPropertyType();
	fwrite(&type, sizeof(EPropertyType), 1, stream);

	switch(type)
	{
	case(PT_FLOAT):
		{
			WriteFloat(property->GetFloat(go));
		}
		break;

	case(PT_COLOR):
		{
			const Color4f& color = property->GetColor(go);
			WriteFloat(color.r);
			WriteFloat(color.g);
			WriteFloat(color.b);
			WriteFloat(color.a);
		}
		break;

	case(PT_INT):
		{
			WriteInt(property->GetInt(go));
		}
		break;
	case(PT_UINT32):
		{
			WriteUInt32(property->GetUInt32(go));
		}
		break;
	case(PT_GUID):
		{
			GUID& id = property->GetGUID(go);
			WriteData(&id, sizeof(GUID), 1);
		}
		break;
	case(PT_HASHCODE):
		{
			WriteUInt32(property->GetHashCode(go));
		}
		break;
	case(PT_HASHSTRING):
		{
			WriteUInt32(property->GetHashString(go));
		}
		break;
	case(PT_BOOL):
		{
			WriteBool(property->GetBool(go));
		}
		break;
	case(PT_STRING):
		{
			const char* text = property->GetString(go);
			WriteString(text);
		}
		break;
	case(PT_VECTOR):
		{
			WriteVector(property->GetVector(go));
		}
		break;
	case(PT_QUATERNION):
		{
			WriteQuat4f(property->GetQuaternion(go));
		}
		break;
	case(PT_EULER):
		{
			const EulerAngle& r = property->GetRotation(go);
			Vector3 v;
			v.x = r.y;
			v.y = r.p;
			v.z = r.r;
			WriteVector(v);
		}
		break;
	case(PT_MATRIX):
		{
			WriteWMatrix16f(property->GetMatrix(go));
		}
		break;
	case(PT_MATRIX3):
		{
			WriteMatrix3(property->GetMatrix3(go));
		}
		break;
	case(PT_BOX):
		{
			const WBox& b=property->GetBox(go);
			WriteVector(b.GetMin());
			WriteVector(b.GetMax());
		}
		break;
	case(PT_OBJECT):
		{
			GenericObject* obj = property->getObject(go);
			if(obj!=NULL)
			{
				WriteObject(obj);
			}
			else
			{
				//write out an integer to indicate no object was set
				WriteInt(-1); 
			}
		}
		break;
	case(PT_ARRAY):
		{
			size_t size = property->GetSize(go);
			WriteUDWORD(size);

			//write out the type (for property type verification)...
			int type = property->GetArrayElementType();
			WriteInt(type);

			for (size_t i = 0; i < size; i++)
			{
				GenericReference<PropertyValue> pv = property->GetValueAt(go, i);
				pv->save(this);
			}
		}
		break;
	case(PT_MAP):
		{
			size_t size = property->GetSize(go);
			WriteUDWORD(size);

			//iterate through all the properties and save out all the map pairs
			FileWriterMapCallback mapIter(this);
			property->IteratePairs(go, &mapIter);
		}
		break;
	case(PT_RESOURCE):
		{
			ResourceObject* pRes=property->GetResource(go);
			WriteResource(pRes);
		}
		break;
	case(PT_CHARARRAY):
		{
			//this slot not even used; TODO: remove
			const char* text = property->GetString(go);
			WriteString(text);
		}
		break;
	default:
		//file writer not implemented for property type
		debug_assert(0,"Property type is unknown.");
		break;
	}
}

void FileWriter::ExportResource(ResourceObject* resource)
{
	HashCodeID hash = resource->GetHashCode();
	WriteULONG(resource->GetType());   //property type (texture, model, etc)
	WriteString(resource->GetName());  //filename
	WriteULONG(hash);

	resource->Serialize(*m_pArchive);
}

void FileWriter::WriteResource(ResourceObject* resource)
{
	bool hasResource = (resource != NULL);
	WriteBool(hasResource);

	if (hasResource)
	{
		HashCodeID hash = resource->GetHashCode();
		WriteULONG(resource->GetType());   //property type (texture, model, etc)
		WriteULONG(hash);

		//write out the package this resource belongs to....
		WriteULONG(resource->GetPackage()->GetPackageID());
	}
}

void FileWriter::WriteGUID(GUID& uid)
{
	fwrite(&uid, sizeof(GUID), 1, stream);
}

void FileWriter::WriteString(const char *text)
{
	//first write out the length of the string
	int len = 0;
	
	if (text)
	{
		len = (int)strlen(text);
	}

	WriteInt(len);

	//serialize the string data
	if (text)
	{
		fwrite(text, sizeof(char), len, stream);
	}
}

void FileWriter::WriteVector(const Vector3& v)
{
	fwrite(&v.x, sizeof(Vector3), 1, stream);
}

void FileWriter::WriteUDWORD(udword &v)
{
	fwrite(&v, sizeof(udword), 1, stream);
}

void FileWriter::WriteUnsignedInt(size_t v)
{
	fwrite(&v, sizeof(size_t), 1, stream);
}

void FileWriter::WriteU16(u16 &us)
{
	fwrite(&us, sizeof(u16), 1, stream);
}

void FileWriter::WriteUInt32(UInt32 us)
{
	fwrite(&us, sizeof(UInt32), 1, stream);
}

void FileWriter::WriteBBox(WBox& bbox)
{
	Vector3 tm1 = bbox.GetMin();
	Vector3 tm2 = bbox.GetMax();

	fwrite(&tm1.x, sizeof(float), 3, stream);
	fwrite(&tm2.x, sizeof(float), 3, stream);
}

void FileWriter::WriteULONG(unsigned long value)
{
	fwrite(&value, sizeof(unsigned long), 1, stream);
}

void FileWriter::WriteStruct(Struct* myStruct, void* data, size_t sizeOf)
{
	std::vector<StructPropertyPair> pairs;
	myStruct->GetPairs(pairs);

	size_t numProperties = pairs.size();

	//Struct type
	WriteUInt32(myStruct->GetHash());
	WriteUInt32(numProperties);
	WriteUInt32(sizeOf);

	for(size_t i = 0; i < numProperties; i++)
	{
		//write name
		WriteUInt32(pairs[i].key);

		//write property type for verification
		GenericReference<PropertyValue> v = pairs[i].value->GetProperty(data);
		EPropertyType t = v->getType();
		WriteInt(t);

		//write value
		v->save(this);
	}
}

}