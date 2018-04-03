/* =================================================================

	===============================================================
*/

#pragma once

#include "PropertyValue.h"
#include "PropertyStruct.h"
#include <CoreFramework/Core/atomic_ref.h>

namespace GODZ
{
	template <class T>
	class ConvertToProperty<GenericReference<T> >
	{
	public:
		ConvertToProperty(ResourceObject* val)
		{
			m_value = new PropertyResource(val);
		}

		operator GenericReference<PropertyValue>() { return m_value; }

	private:
		GenericReference<PropertyValue> m_value;
	};

	template <class T>
	class ConvertFromProperty<GenericReference<T> >
	{
	public:
		ConvertFromProperty(GenericReference<PropertyValue> val)
		{
			if (val->getType() == PT_RESOURCE)
			{
				m_value = (T*)( val->getResource() );
			}
		}

		operator GenericReference<T>() { return m_value; }

	private:
		GenericReference<T> m_value;
	};

	template <class T>
	class ConvertToProperty<atomic_ptr<T> >
	{
	public:
		ConvertToProperty(const atomic_ptr<GenericObject>& val)
		{
			m_value = new PropertyObject(val);
		}

		operator GenericReference<PropertyValue>() { return m_value; }

	private:
		GenericReference<PropertyValue> m_value;
	};

	template <class T>
	class ConvertFromProperty<atomic_ptr<T> >
	{
	public:
		ConvertFromProperty(GenericReference<PropertyValue> val)
		{
			if (val->getType() == PT_OBJECT)
			{
				PropertyObject* po = (PropertyObject*)val.m_ref;
				m_value = atomic_ptr_cast<T>( po->getPtr() );
			}
		}

		operator atomic_ptr<T>() { return m_value; }

	private:
		atomic_ptr<T> m_value;
	};

	template <class T>
	class ConvertToProperty<T*>
	{
	public:
		ConvertToProperty(const atomic_ptr<GenericObject>& val)
		{
			m_value = new PropertyObject(val);
		}
		ConvertToProperty(ResourceObject* val)
		{
			m_value = new PropertyResource(val);
		}

		operator GenericReference<PropertyValue>() { return m_value; }

	private:
		GenericReference<PropertyValue> m_value;
	};

	template <class T>
	class ConvertFromProperty<T*>
	{
	public:
		ConvertFromProperty(GenericReference<PropertyValue> val)
		{
			if (val->getType() == PT_OBJECT)
			{
				PropertyObject* po = (PropertyObject*)val.m_ref;
				m_value = atomic_ptr_cast<T>( po->getPtr() );
			}
		}

		operator T*() { return m_value; }

	private:
		atomic_ptr<T> m_value;
	};


	template <>
	class ConvertFromProperty<ResourceObject*>
	{
	public:
		ConvertFromProperty(GenericReference<PropertyResource> val)
		{
			if (val->getType() == PT_RESOURCE)
			{
				m_resource = ( val->getResource() );
			}
		}

		operator ResourceObject*() { return m_resource; }

	private:
		GenericReference<ResourceObject> m_resource;
	};

	template <>
	class ConvertToProperty<ResourceObject*>
	{
	public:
		ConvertToProperty(ResourceObject* val)
		{
			m_value = new PropertyResource(val);
		}

		operator GenericReference<PropertyValue>() { return m_value; }

	private:
		GenericReference<PropertyValue> m_value;
	};


	template <>
	class ConvertToProperty<int>
	{
	public:
		ConvertToProperty(int val)
		{
			m_value = new PropertyInt(val);
		}

		operator GenericReference<PropertyValue>() { return m_value; }

	private:
		GenericReference<PropertyValue> m_value;
	};

	template <>
	class ConvertFromProperty<int>
	{
	public:
		ConvertFromProperty(GenericReference<PropertyValue> val)
		{
			m_value = val->getInt();
		}

		operator int() { return m_value; }

	private:
		int m_value;
	};

	template <>
	class ConvertToProperty<float>
	{
	public:
		ConvertToProperty(float val)
		{
			m_value = new PropertyFloat(val);
		}

		operator GenericReference<PropertyValue>() { return m_value; }

	private:
		GenericReference<PropertyValue> m_value;
	};

	template <>
	class ConvertFromProperty<float>
	{
	public:
		ConvertFromProperty(GenericReference<PropertyValue> val)
		{
			m_value = val->getFloat();
		}

		operator float() { return m_value; }

	private:
		float m_value;
	};


	template <>
	class ConvertToProperty<UInt32>
	{
	public:
		ConvertToProperty(UInt32 val)
		{
			m_value = new PropertyUInt32(val);
		}

		operator GenericReference<PropertyValue>() { return m_value; }

	private:
		GenericReference<PropertyValue> m_value;
	};

	template <>
	class ConvertFromProperty<UInt32>
	{
	public:
		ConvertFromProperty(GenericReference<PropertyValue> val)
		{
			m_value = val->getUInt32();
		}

		operator UInt32() { return m_value; }

	private:
		UInt32 m_value;
	};


	/*
	* Utility used to datamine the Class type associated with a property
	*/
	template <class T>
	class GetClass
	{
	};

	template <class T>
	class GetClass<GenericReference<T> >
	{
	public:
		GetClass()
		{
			m_value = T::GetClass();
		}

		operator GenericClass*() { return m_value; }

	private:
		GenericClass* m_value;
	};

	template <class T>
	class GetClass<atomic_ptr<T> >
	{
	public:
		GetClass()
		{
			m_value = T::GetClass();
		}

		operator GenericClass*() { return m_value; }

	private:
		GenericClass* m_value;
	};

	template <class T>
	class GetClass<T*>
	{
	public:
		GetClass()
		{
			m_value = T::GetClass();
		}

		operator GenericClass*() { return m_value; }

	private:
		GenericClass* m_value;
	};

	template <>
	class GetClass<int>
	{
	public:
		GetClass()
		{
		}

		operator GenericClass*() { return NULL; }
	};

	template <>
	class GetClass<UInt32>
	{
	public:
		GetClass()
		{
		}

		operator GenericClass*() { return NULL; }
	};

	template <>
	class GetClass<float>
	{
	public:
		GetClass()
		{
		}

		operator GenericClass*() { return NULL; }
	};

	template <>
	class GetClass<ulong>
	{
	public:
		GetClass()
		{
		}

		operator GenericClass*() { return NULL; }
	};

	/*
	* Utility used to datamine the property type
	*/
	template <class T>
	class GetElementType
	{
	};

	template <>
	class GetElementType<int>
	{
	public:
		operator EPropertyType() { return PT_INT; }
	};

	template <>
	class GetElementType<float>
	{
	public:
		operator EPropertyType() { return PT_FLOAT; }
	};

	template <>
	class GetElementType<UInt32>
	{
	public:
		operator EPropertyType() { return PT_UINT32; }
	};

	template <class T>
	class GetElementType<GenericReference<T> >
	{
	public:
		operator EPropertyType() { return PT_OBJECT; }
	};

	template <class T>
	class GetElementType<atomic_ptr<T> >
	{
	public:
		operator EPropertyType() { return PT_OBJECT; }
	};

	template <class T>
	class GetElementType<T*>
	{
	public:
		operator EPropertyType() { return PT_OBJECT; }
	};


	///////////////////////////////////////////////////////////////////////////////////////

	template<class T> void SetValue( T& val, GenericReference<PropertyValue> value )
	{
		val = ConvertFromProperty<T>( value );
	}

	template <class V> GenericReference<PropertyValue> GetValue( V value )
	{
		return ConvertToProperty< V >( value );
	}

	template <class V> EPropertyType GetPropertyType( V value )
	{
		return GetElementType< V >();
	}
}

