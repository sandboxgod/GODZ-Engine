/* ===========================================================

	========================================================== 
*/

#include "ClassProperty.h"
#include "DataConversions.h"
#include <CoreFramework/Core/ClassRegistry.h>
#include <CoreFramework/Core/Color4f.h>
#include <CoreFramework/Core/GenericNode.h>
#include <CoreFramework/Core/GenericPackage.h>
#include <CoreFramework/Reflection/GenericClass.h>
#include <CoreFramework/Core/ResourceManager.h>
#include <CoreFramework/Core/StringTokenizer.h>
#include <CoreFramework/Math/Quaternion.h>
#include <CoreFramework/Math/EulerAngleConverter.h>
#include <Rpc.h>
//#include <AtlConv.h>
//#include <AtlBase.h>


namespace GODZ
{
static const int MAX_NUM_PROPERTIES = 128;
static unsigned int property_count = 0;
ClassProperty* mPropertyList[MAX_NUM_PROPERTIES];

ClassProperty::ClassProperty(const char* propertyName, EPropertyType type)
	: mName(propertyName)
	, mType(type)
	, mHash(propertyName)
{
	godzassert (property_count < MAX_NUM_PROPERTIES); //need to increase array size

	mPropertyList[property_count] = this;
	property_count++;
}

ClassProperty::ClassProperty(const char* propertyName, EPropertyType type, HString classType)
	: mName(propertyName)
	, mType(type)
	, mHash(propertyName)
	, mClassType(classType)
{
	mPropertyList[property_count] = this;
	property_count++;

	godzassert (property_count < MAX_NUM_PROPERTIES); //need to increase array size
}

void ClassProperty::RegistryProperties()
{
	for(UInt32 i=0;i<property_count;i++)
	{
		mPropertyList[i]->Register();
	}
}

rstring ClassProperty::GetText(GenericObject* object)
{
	rstring text;

	switch(mType)
	{
	default:
		return NULL_STRING;
		break;
	case PT_GUID:
		{
			GUID& id = GetGUID(object);

			char dest[255];
			const int GUID_STRING_LEN = 45;
			OLECHAR szGuid[GUID_STRING_LEN]={0};
			int nCount = ::StringFromGUID2(id, szGuid, GUID_STRING_LEN);

			//http://msdn.microsoft.com/en-us/library/87zae4a3(VS.80).aspx
			//text = CW2A( szGuid );
			wcstombs(dest, szGuid, GUID_STRING_LEN);
			text = dest;
		}
		break;

	case PT_BOOL:
		{
			bool b = GetBool(object);
			if (b)
			{
				text = "true";
			}
			else
			{
				text = "false";
			}
		}
		break;
	case PT_INT:
		{
			int value = GetInt(object);
			const int bufferSize = 1024;
			char buf[bufferSize];
			_itoa_s(value, buf, bufferSize, 10);
			text = buf;
			return text;
		}
		break;
	case PT_UINT32:
		{
			const int bufferSize = 1024;
			UInt32 value = GetUInt32(object);
			char buf[bufferSize];
			_ultoa_s(value, buf, bufferSize, 10);
			text = buf;
			return text;
		}
		break;
	case PT_FLOAT:
		{
			return GetFloatAsString(GetFloat(object));
		}
		break;
	case PT_VECTOR:
		{
			const Vector3& v = GetVector(object);
			text=GetVectorAsString(v);
			return text;
		}
		break;
	case PT_COLOR:
		{
			Color4f color = GetColor(object);
			float f[4];
			f[0] = color.r;
			f[1] = color.g;
			f[2] = color.b;
			f[3] = color.a;

			text=GetFloatArrayAsString(&f[0], 4);
			return text;
		}
		break;

	case PT_QUATERNION:
		{
			const Quaternion& q = GetQuaternion(object);
			float f[4];
			f[0] = q.q.x;
			f[1] = q.q.y;
			f[2] = q.q.z;
			f[3] = q.q.w;

			text=GetFloatArrayAsString(&f[0], 4);
			return text;
		}
		break;

	case PT_EULER:
		{
			EulerAngle v = GetRotation(object);
			text=GetRotatorAsString(v);
			return text;
		}
		break;
	case PT_MATRIX:
		{
			const WMatrix16f & mat = GetMatrix(object);

			//TODO: Remove scale from matrix or notify user
			//this matrix may not be pure rotation matrix

			EulerAngle euler = EulerAngleConverter::FromMatrix4(mat);

			float rot[3];
			rot[0] = euler.y;
			rot[1] = euler.p;
			rot[2] = euler.r;

			text=GetFloatArrayAsString(rot, 3);
			text += " ";
			text += GetVectorAsString(mat.GetTranslation());
			return text;
		}
		break;
	case PT_MATRIX3:
		{
			const Matrix3 & mat = GetMatrix3(object);

			EulerAngle euler = EulerAngleConverter::FromMatrix3(mat);

			float rot[3];
			rot[0] = euler.y;
			rot[1] = euler.p;
			rot[2] = euler.r;

			text = GetFloatArrayAsString(rot, 3);
			return text;
		}
		break;
	case PT_STRING:
		{
			return GetString(object);
		}
		break;
	case PT_BOX:
		{
			WBox b = GetBox(object);
			text = "min: ";
			text += GetVectorAsString(b.GetMin());
			text += " max:";
			text += GetVectorAsString(b.GetMax());
			return text;
		}
		break;
	case PT_RESOURCE:
		{
			ResourceObject* pRes = GetResource(object);
			if (!pRes)
			{
				return "";
			}

			//returns <package name>.<resource name>
			GenericPackage* p = pRes->GetPackage();
			if (p != NULL)
			{
				text = p->GetName();
				text += ".";
			}

			text += pRes->GetName();
		}
		break;
	case PT_OBJECT:
		{
			//We can crash hard if a Class is passed in here due to the way we cast the object
			//pointer in ClassProperty::getObject(obj)
			//godzassert (object->IsAObject());

			//return object->ToString();
		}
		break;
	case PT_ARRAY:
		{
			size_t size = GetSize(object);
			text += GODZ::GetString("(%d Objects)", size);

			return text;
		}
		break;
	case PT_MAP:
		{
			size_t size = GetSize(object);
			text += GODZ::GetString("(%d Objects)", size);

			return text;
		}
		break;
	}

	return text;
}

bool ClassProperty::SetText(GenericObject* object, const char* text)
{
	switch(mType)
	{
	default:
		{
			//Not supported for this type
			godzassert(0);
		}
		break;
	case PT_BOOL:
		{
			if (strcmp(text,"true")==0)
			{
				SetBool(object, true);
			}
			else
			{
				SetBool(object, false);
			}
		}
		break;
	case PT_INT:
		{
			SetInt(object, atoi(text) );
		}
		break;
	case PT_UINT32:
		{
			SetUInt32(object, _atoi64(text) );
		}
		break;
	case PT_HASHSTRING:
		{
			if (_stricmp(text, NULL_STRING) != 0)
			{
				SetHashString(object, text );
			}
			else
			{
				SetHashString(object, HString() );
			}
		}
		break;
	case PT_HASHCODE:
		{
			if (_stricmp(text, NULL_STRING) != 0)
				SetHashCode(object, _atoi64(text) );
			else
			{
				SetHashCode(object, 0 );
			}
		}
		break;
	case PT_FLOAT:
		{
			float val = (float)atof(text);
			SetFloat(object, val);
		}
		break;
	case PT_VECTOR:
		{
			Vector3 v;
			GetStringAsVector(text, v);
			SetVector(object, v);
		}
		break;
	case PT_COLOR:
		{
			float v[4];
			GetStringAsFloatArray(text, &v[0], 4);

			Color4f c;
			c.r = v[0];
			c.g = v[1];
			c.b = v[2];
			c.a = v[3];
			SetColor(object, c);
		}
		break;

	case PT_QUATERNION:
		{
			float v[4];
			GetStringAsFloatArray(text, &v[0], 4);

			Quaternion q;
			q.q.x = v[0];
			q.q.y = v[1];
			q.q.z = v[2];
			q.q.w = v[3];
			SetQuaternion(object, q);
		}
		break;

	case PT_EULER:
		{
			EulerAngle ptr;
			GetStringAsRotator(text, ptr);
			SetRotation(object, ptr);
		}
		break;
	case PT_MATRIX:
		{
			WMatrix16f mat;
			GetStringAsMatrix(text, mat);
			SetMatrix(object, mat);
		}
		break;
	case PT_MATRIX3:
		{
			EulerAngle euler;
			GetStringAsRotator(text, euler);

			Matrix3 mat;
			mat.Rotate( Vector3( euler.r, euler.p, euler.y ) );
			SetMatrix(object, mat);
		}
		break;
	case PT_STRING:
		{
			SetString(object, text);
		}
		break;
	case PT_BOX:
		{
			WBox b;
			GetStringAsBox(text, b);
			SetBox(object, b);
		}
		break;
	case PT_RESOURCE:
		{
			ResourceManager* rmngr = ResourceManager::GetInstance();

			if (strlen(text) > 0)
			{
				SetResource(object, rmngr->findResource(text));

				//We search the resource manager for a resource that matches that name. If none can be
				//found, we exit. That simple....
			}
			else
			{
				SetResource(object, NULL);
			}
		}
		break;
	case PT_GUID:
	case PT_OBJECT:
		{
		}
		break;
	case PT_MAP:
	case PT_ARRAY:
		{
		}
		break;
	}


	return true;
}

const Color4f& ClassProperty::GetColor(GenericObject* object)
{
	return ZeroColor;
}


const Vector3& ClassProperty::GetVector(GenericObject* object)
{
	return ZERO_VECTOR;
}

const Quaternion& ClassProperty::GetQuaternion(GenericObject* object)
{
	static Quaternion zero(0,0,0,1);
	return zero;
}

EulerAngle ClassProperty::GetRotation(GenericObject* object)
{
	return ZERO_ROTATION;
}

const WBox& ClassProperty::GetBox(GenericObject* object)
{
	return EMPTY_BOX;
}

const WMatrix& ClassProperty::GetMatrix(GenericObject* object)
{
	return IDENTITY_MATRIX;
}

const WMatrix3& ClassProperty::GetMatrix3(GenericObject* object)
{
	static Matrix3 matIdentity(1);
	return matIdentity;
}

GenericClass* ClassProperty::GetClassType()
{
	return ClassRegistry::findClass(mClassType);
}

GenericReference<PropertyValue> ClassProperty::GetValueAt(GenericObject* object, size_t index)
{
	return NULL;
}

GUID& ClassProperty::GetGUID(GenericObject* object)
{
	static GUID g = GUID_NULL;
	return g;
}


}

