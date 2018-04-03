/* =================================================================
	ClassProperty

	Added Jan 2, 10
	===============================================================
*/

#if !defined(_CLASSPROPERTY_H_)
#define _CLASSPROPERTY_H_

#include <CoreFramework/Core/CoreFramework.h>
#include <CoreFramework/Core/HString.h>
#include <CoreFramework/Core/Color4f.h>
#include <CoreFramework/Core/GodzVector.h>
#include <CoreFramework/Math/WMatrix.h>
#include <CoreFramework/Math/Quaternion.h>
#include <CoreFramework/Math/WBox.h>
#include <CoreFramework/Math/WMatrix3.h>
#include <CoreFramework/Math/EulerAngle.h>
#include "PropertyConvertReference.h"
#include <hash_map>

namespace GODZ
{

#define REGISTER_INT(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetInt(GenericObject* object, int value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		int GetInt(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_INT); \

#define REGISTER_UINT32(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetUInt32(GenericObject* object, UInt32 value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		UInt32 GetUInt32(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_UINT32); \

#define REGISTER_UINT32_OBSOLETE(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetUInt32(GenericObject* object, UInt32 value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		UInt32 GetUInt32(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual bool IsObsolete() { return true; }				\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_UINT32); \


#define REGISTER_GUID(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetGUID(GenericObject* object, const GUID& value)						\
		{														\
			((c*)object)->Set##p(value);						\
		}														\
		GUID& GetGUID(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_GUID); \

#define REGISTER_HASHCODE(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetHashCode(GenericObject* object, HashCodeID value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		HashCodeID GetHashCode(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_HASHCODE); \

#define REGISTER_FLOAT(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetFloat(GenericObject* object, float value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		float GetFloat(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_FLOAT); \


#define REGISTER_COLOR(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetColor(GenericObject* object, const Color4f& value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		const Color4f& GetColor(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_COLOR); \

#define REGISTER_EULER_ANGLE(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetRotation(GenericObject* object, const EulerAngle& value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		EulerAngle GetRotation(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_VECTOR); \

#define REGISTER_VECTOR(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetVector(GenericObject* object, const Vector3& value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		const Vector3& GetVector(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_VECTOR); \

#define REGISTER_VECTOR_DEPRECATED(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetVector(GenericObject* object, const Vector3& value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		const Vector3& GetVector(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual bool IsObsolete() { return true; }				\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_VECTOR); \

#define REGISTER_QUATERNION(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetQuaternion(GenericObject* object, const Quaternion& value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		const Quaternion& GetQuaternion(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_QUATERNION); \

#define REGISTER_BOX(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetBox(GenericObject* object, const WBox& value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		const WBox& GetBox(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_BOX); \

#define REGISTER_BOOL(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetBool(GenericObject* object, bool value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		bool GetBool(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_BOOL); \


#define REGISTER_RESOURCE(c, p, objecttype)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetResource(GenericObject* object, ResourceObject* value)						\
		{														\
			((c*)object)->Set##p((objecttype*)value);								\
		}														\
		ResourceObject* GetResource(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_RESOURCE); \

#define REGISTER_RESOURCE_OBSOLETE(c, p, objecttype)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetResource(GenericObject* object, ResourceObject* value)						\
		{														\
			((c*)object)->Set##p((objecttype*)value);								\
		}														\
		ResourceObject* GetResource(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual bool IsObsolete() { return true; }				\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_RESOURCE); \

#define REGISTER_OBJECT(c, p, objecttype)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type, HString hash)		\
		: ClassProperty(propertyName, type, hash)						\
		{														\
		}														\
		void setObject(GenericObject* object, atomic_ptr<GenericObject>& value)						\
		{														\
			((c*)object)->Set##p( atomic_ptr_cast<objecttype>(value) );								\
		}														\
		atomic_ptr<GenericObject> getObject(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();						\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_OBJECT, HString(""#objecttype"") ); \

#define REGISTER_OBJECT_DEPRECATED(c, p, objecttype)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type, HString hash)		\
		: ClassProperty(propertyName, type, hash)						\
		{														\
		}														\
		void setObject(GenericObject* object, atomic_ptr<objecttype>& value)						\
		{														\
			((c*)object)->Set##p( atomic_ptr_cast<objecttype>(value) );							\
		}														\
		atomic_ptr<GenericObject> getObject(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual bool IsObsolete() { return true; }				\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_OBJECT, HString(""#objecttype"") ); \

#define REGISTER_STRING(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetString(GenericObject* object, const char* value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		const char* GetString(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_STRING); \

#define REGISTER_HASHSTRING(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetHashString(GenericObject* object, HString value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		HString GetHashString(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_HASHSTRING); \

#define REGISTER_MATRIX(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetMatrix(GenericObject* object, const WMatrix& value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		const WMatrix& GetMatrix(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_MATRIX); \

#define REGISTER_MATRIX3(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		void SetMatrix3(GenericObject* object, const Matrix3& value)						\
		{														\
			((c*)object)->Set##p(value);								\
		}														\
		const Matrix3& GetMatrix3(GenericObject* object)									\
		{														\
			return ((c*)object)->Get##p();							\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_MATRIX3); \

// Expects the user to use GodzVector or another type of compatiable vector
// that has remove()
#define REGISTER_ARRAY(c, p, objecttype)					\
	class PrivateClassProperty##c##p : public ClassProperty	\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		GenericReference<PropertyValue> GetValueAt(GenericObject* object, size_t index)		\
		{														\
			return ConvertToProperty< objecttype >( ( ((c*)object)->Get##p())[index] );				\
		}														\
		void AddValue(GenericObject* object, GenericReference<PropertyValue> value)		\
		{														\
			(((c*)object)->Get##p()).push_back( ConvertFromProperty< objecttype >(value) );		\
		}														\
		void SetValueAt(GenericObject* object, GenericReference<PropertyValue> value, size_t index)		\
		{														\
			(((c*)object)->Get##p())[index] = ConvertFromProperty< objecttype >(value);		\
		}														\
		EPropertyType GetArrayElementType()									\
		{														\
			return GetElementType< objecttype >();								\
		}														\
		size_t GetSize(GenericObject* object)						\
		{									\
			return ( ((c*)object)->Get##p()).size();			\
		}									\
		void Clear(GenericObject* object)						\
		{									\
			return ( ((c*)object)->Get##p()).clear();			\
		}									\
		bool RemoveValue(GenericObject* object, GenericReference<PropertyValue> value)						\
		{									\
			return ( ((c*)object)->Get##p()).remove(ConvertFromProperty< objecttype >(value));			\
		}									\
		GenericClass* GetClassType()			\
		{						\
			return GetClass< objecttype >();				\
		}						\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_ARRAY ); \

//Specialized Array declaration required to declare a collection of ResourceObjects
#define REGISTER_RESOURCE_ARRAY(c, p, objecttype)					\
	class PrivateClassProperty##c##p : public ClassProperty	\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		GenericReference<PropertyValue> GetValueAt(GenericObject* object, size_t index)		\
		{														\
			return ConvertToProperty< objecttype >( ( ((c*)object)->Get##p())[index] );				\
		}														\
		void AddValue(GenericObject* object, GenericReference<PropertyValue> value)		\
		{														\
			(((c*)object)->Get##p()).push_back( ConvertFromProperty< objecttype >(value) );		\
		}														\
		void SetValueAt(GenericObject* object, GenericReference<PropertyValue> value, size_t index)		\
		{														\
			(((c*)object)->Get##p())[index] = ConvertFromProperty< objecttype >(value);		\
		}														\
		size_t GetSize(GenericObject* object)						\
		{									\
			return ( ((c*)object)->Get##p()).size();			\
		}									\
		void Clear(GenericObject* object)						\
		{									\
			return ( ((c*)object)->Get##p()).clear();			\
		}									\
		bool RemoveValue(GenericObject* object, GenericReference<PropertyValue> value)						\
		{									\
			return ( ((c*)object)->Get##p()).remove(ConvertFromProperty< objecttype >(value));			\
		}									\
		EPropertyType GetArrayElementType()									\
		{														\
			return PT_RESOURCE;								\
		}														\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_ARRAY ); \

#define REGISTER_MAP(c, p)					\
	class PrivateClassProperty##c##p : public ClassProperty		\
	{															\
	public:														\
		PrivateClassProperty##c##p(const char* propertyName, EPropertyType type)		\
		: ClassProperty(propertyName, type)						\
		{														\
		}														\
		size_t GetSize(GenericObject* object)						\
		{									\
			return ( ((c*)object)->Get##p()).size();			\
		}									\
		void Clear(GenericObject* object)						\
		{									\
			return ( ((c*)object)->Get##p()).clear();			\
		}									\
		void AddElement(GenericObject* object, GenericReference<PropertyValue> key, GenericReference<PropertyValue> value)		\
		{									\
			AddMapElement( ((c*)object)->Get##p(), key, value);			\
		}									\
		GenericReference<PropertyValue> GetElement(GenericObject* object, GenericReference<PropertyValue> key)		\
		{									\
			return GetMapElement( ((c*)object)->Get##p(), key);			\
		}									\
		void IteratePairs(GenericObject* object, MapCallback* callback)						\
		{									\
			IterateMapElements( ((c*)object)->Get##p(), callback);			\
		}									\
		EPropertyType GetKeyElementType(GenericObject* object)		\
		{									\
			return GetMapKeyElementType( ((c*)object)->Get##p());			\
		}									\
		EPropertyType GetValueElementType(GenericObject* object)		\
		{									\
			return GetMapValueElementType( ((c*)object)->Get##p());			\
		}									\
		virtual void Register()									\
		{														\
			PrivateRuntime##c::_Class->AddProperty(this);		\
		}														\
	};															\
	static PrivateClassProperty##c##p _registerProperty##c##p(""#p"", PT_MAP); \

	//forward decl
	class GenericClass;

	class GODZ_API MapCallback
	{
	public:
		virtual void analyzeMapPair(GenericReference<PropertyValue> key, GenericReference<PropertyValue> value) = 0;
	};

	//Used to copy over Map Property values
	class PropertyStoreCallback : public MapCallback
	{
	public:
		virtual void analyzeMapPair(GenericReference<PropertyValue> key, GenericReference<PropertyValue> value)
		{
			mKeys.push_back(key);
			mValues.push_back(value);
		}

		std::vector<GenericReference<PropertyValue> > mKeys;
		std::vector<GenericReference<PropertyValue> > mValues;
	};

	template <class V, class K> void AddMapElement( stdext::hash_map<V,K>& map, GenericReference<PropertyValue> key, GenericReference<PropertyValue> value )
	{
		map[ ConvertFromProperty< V >( key ) ] = ConvertFromProperty< K >( value );
	}

	template <class V, class K> GenericReference<PropertyValue> GetMapElement( stdext::hash_map<V,K>& map, GenericReference<PropertyValue> key )
	{
		return ConvertToProperty< K >( map[ ConvertFromProperty< V >( key ) ] );
	}

	template <class V, class K> void IterateMapElements( stdext::hash_map<V,K>& map, MapCallback* callback )
	{
		stdext::hash_map<V,K>::iterator iter;
		for (iter = map.begin(); iter != map.end(); iter++)
		{
			callback->analyzeMapPair( ConvertToProperty< V >( iter->first ), ConvertToProperty< K >( iter->second ) );
		}
	}

	template <class V, class K> EPropertyType GetMapKeyElementType( stdext::hash_map<V,K>& map )
	{
		return GetElementType< V >();
	}

	template <class V, class K> EPropertyType GetMapValueElementType( stdext::hash_map<V,K>& map )
	{
		return GetElementType< K >();
	}

	/*
	* Represents a property that is owned by a Class
	*/
	class GODZ_API ClassProperty
	{
	public:
		ClassProperty(const char* propertyName, EPropertyType type);
		ClassProperty(const char* propertyName, EPropertyType type, HString classType);

		//Registers this property with it's class....
		virtual void Register() = 0;

		EPropertyType GetPropertyType() { return mType; }
		HashCodeID GetHashCode() { return mHash; }

		//Marks this property as obsolete. We can still use it like normal however it
		//shouldn't be serialized anymore. This allows us to more easily convert
		//over serialized files
		virtual bool IsObsolete() { return false; }

		//for editor
		const char* GetPropertyName() { return mName; }

		//ObjectProperties also store an additional Class restriction that can be used
		virtual GenericClass* GetClassType();

		//converts text string into a property
		bool SetText(GenericObject* object, const char* text);
		rstring GetText(GenericObject* object);

		//Registers all the pending properties
		static void RegistryProperties();

		virtual void SetGUID(GenericObject* object, const GUID& value) {}
		virtual GUID& GetGUID(GenericObject* object);

		virtual void SetFloat(GenericObject* object, float value) {}
		virtual float GetFloat(GenericObject* object) { return 0; }

		virtual void SetInt(GenericObject* object, int value) {}
		virtual int GetInt(GenericObject* object) { return 0; }

		virtual void SetULong(GenericObject* object, ulong value) {}
		virtual int GetULong(GenericObject* object) { return 0; }

		virtual void SetHashCode(GenericObject* object, HashCodeID value) {}
		virtual HashCodeID GetHashCode(GenericObject* object) { return 0; }

		virtual void SetHashString(GenericObject* object, HString value) {}
		virtual HString GetHashString(GenericObject* object) { return HString(); }

		virtual void SetUInt32(GenericObject* object, UInt32 value) {}
		virtual UInt32 GetUInt32(GenericObject* object) { return 0; }

		virtual void SetResource(GenericObject* object, ResourceObject* value) {}
		virtual ResourceObject* GetResource(GenericObject* object) { return NULL; }

		virtual void SetColor(GenericObject* object, const Color4f& value) {}
		virtual const Color4f& GetColor(GenericObject* object);

		virtual void SetVector(GenericObject* object, const Vector3& value) {}
		virtual const Vector3& GetVector(GenericObject* object);

		virtual void SetQuaternion(GenericObject* object, const Quaternion& value) {}
		virtual const Quaternion& GetQuaternion(GenericObject* object);

		virtual void SetBool(GenericObject* object, bool value) {}
		virtual bool GetBool(GenericObject* object) { return false; }

		virtual void setObject(GenericObject* object, atomic_ptr<GenericObject>& value) {}
		virtual atomic_ptr<GenericObject> getObject(GenericObject* object) { return atomic_ptr<GenericObject>(); }

		virtual void SetRotation(GenericObject* object, const EulerAngle& value) {}
		virtual EulerAngle GetRotation(GenericObject* object);

		virtual void SetBox(GenericObject* object, const WBox& value) {}
		virtual const WBox& GetBox(GenericObject* object);

		virtual void SetString(GenericObject* object, const char* value) {}
		virtual const char* GetString(GenericObject* object) { return ""; }

		virtual void SetMatrix(GenericObject* object, const WMatrix& value) {}
		virtual const WMatrix& GetMatrix(GenericObject* object);

		virtual void SetMatrix3(GenericObject* object, const Matrix3& value) {}
		virtual const Matrix3& GetMatrix3(GenericObject* object);

		//Array interface functions....
		virtual GenericReference<PropertyValue> GetValueAt(GenericObject* object, size_t index);
		virtual void AddValue(GenericObject* object, GenericReference<PropertyValue> value) {}
		virtual void SetValueAt(GenericObject* object, GenericReference<PropertyValue> value, size_t index) {}
		virtual bool RemoveValue(GenericObject* object, GenericReference<PropertyValue> value) { return false; }
		virtual EPropertyType GetArrayElementType() { return PT_UNKNOWN; }

		//Array & Map interface functions...
		virtual size_t GetSize(GenericObject* object) { return 0; }
		virtual void Clear(GenericObject* object) {}

		//Map interface functions...
		virtual void AddElement(GenericObject* object, GenericReference<PropertyValue> key, GenericReference<PropertyValue> value) {}
		virtual GenericReference<PropertyValue> GetElement(GenericObject* object, GenericReference<PropertyValue> key) { return NULL; }
		virtual void IteratePairs(GenericObject* object, MapCallback* callback) {}
		virtual EPropertyType GetKeyElementType(GenericObject* object) { return PT_UNKNOWN; }
		virtual EPropertyType GetValueElementType(GenericObject* object) { return PT_UNKNOWN; }

	private:
		EPropertyType	mType;
		HString			mHash;
		rstring			mName;
		HString			mClassType;
	};
}

#endif //
