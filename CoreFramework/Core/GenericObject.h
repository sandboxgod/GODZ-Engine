/* =================================================================
	Defines the GenericObject base class. Generic Objects are 
	'instance' counted/serializable classes. Every subclass of 
	GenericObject should be declared by the DeclareGeneric(Class) 
	macro. Within the implementation file for the class (.cpp), you 
	must use a matching ImplementGeneric(Class) macro. This 
	capability gives the engine an advanced RTTI system capable of 
	loading class instances by name, performing IsA() tests, and 
	automatic package registration -the engine is able to figure out 
	what DLLs classes belong to (see class GenericPackage).

	Created Nov 9, 2003 by Richard Osborne
	Copyright (c) 2003, Richard Osborne
	===============================================================
*/

#if !defined(_GenericObject_H_)
#define _GenericObject_H_

#include "Godz.h"
#include "RString.h"
#include "IWriter.h"
#include "atomic_ref.h"
#include <Guiddef.h>

// These macros are used to 'register' generic objects so that
// the World Editor can dynamically discover objects.

namespace GODZ
{
	class GenericObject;

	//Class Flags
	enum GODZ_API ClassFlag
	{
		ClassFlag_Placeable = 1<<1,  // Objects spawned by this type of class should be placeable in the World
		ClassFlag_Transient = 1<<2,	 // Objects of this type should not be serialized
		ClassFlag_Abstract = 1<<3,	// This class should not be directly instantiated...
	};

	//Stores GenericObject - special use by the Engine Framework
	GODZ_API void PushGenericObject(GenericObject* pObj);

//Should only be used if the Class doesn't inherit from anything
#define DeclareGenericRoot(N_CLASS) \
	private:\
	static GODZ::GenericClass* PrivateGenericClass; static GODZ::GenericClass* PrivateGenericClass##N_CLASS(const char* ClassName=CLASS_TO_STR(N_CLASS), GenericClass* parent=NULL); \
	public: \
	static GODZ::GenericClass* GetClass() \
	{ \
		if (PrivateGenericClass == NULL) \
		{ PrivateGenericClass = PrivateGenericClass##N_CLASS(CLASS_TO_STR(N_CLASS)); } \
		return PrivateGenericClass; \
	} \
	virtual GODZ::GenericClass* GetRuntimeClass(); \
	static N_CLASS* StaticConstructObject(const char *Name, GenericObject *Owner=NULL) \
	{ \
		N_CLASS *obj = new N_CLASS(); \
		return obj; \
	} \

//Declares an abstract genericobject subclass.
//GetClass() returns the Static Class Instance
//GetRuntimeClass() is a virtual function that will determine the runtime class definition of an object
#define DeclareGenericBase(N_CLASS, PARENT_CLASS) \
	private:\
	static GODZ::GenericClass* PrivateGenericClass; static GODZ::GenericClass* PrivateGenericClass##N_CLASS(const char* ClassName=CLASS_TO_STR(N_CLASS), GenericClass* parent=NULL); \
	public: \
	static GODZ::GenericClass* GetClass() \
	{ \
		if (PrivateGenericClass == NULL) \
		{ PrivateGenericClass = PrivateGenericClass##N_CLASS(CLASS_TO_STR(N_CLASS), PARENT_CLASS::GetClass()); } \
		return PrivateGenericClass; \
	} \
	virtual GODZ::GenericClass* GetRuntimeClass(); \

//Declares a concrete genericobject subclass
#define DeclareGeneric(N_CLASS, PARENT_CLASS) \
	DeclareGenericBase(N_CLASS, PARENT_CLASS) \
	static N_CLASS* StaticConstructObject(const char *Name, GenericObject *Owner=NULL) \
	{ \
		N_CLASS *obj = new N_CLASS(); \
		return obj; \
	} \

//Declares the 'static' class instance and registers the Generic Class
//object in the global registry (see GenericObject::AllObjects)
#define ImplementGenericFlags(N_CLASS, classFlag) \
	GODZ::GenericClass* N_CLASS::PrivateGenericClass = NULL; \
	class __declspec(dllexport) PrivateRuntime##N_CLASS : public GenericClass \
	{ public: \
		static GODZ::GenericClass* _Class;\
		typedef N_CLASS DynamicClass; \
		PrivateRuntime##N_CLASS(const char* _Name, GenericClass* parent=NULL) \
		{ \
			_Class = this; \
			ClassFlags = classFlag; \
			Setup(_Name, parent); \
		} \
		GenericObject* GetInstance(const char* ObjectName=NULL, GenericClass* parent=NULL) \
		{ \
			GenericObject* obj = N_CLASS::StaticConstructObject(ObjectName,parent); \
			return obj; \
		} \
		virtual GODZ::GenericClass* GetRuntimeClass() \
		{ \
		return _Class; \
		} \
	}; \
	GODZ::GenericClass* PrivateRuntime##N_CLASS::_Class = NULL; \
	GODZ::GenericClass* N_CLASS::PrivateGenericClass##N_CLASS(const char* _Name, GenericClass* parent) \
	{ \
	GenericClass* gClass = new PrivateRuntime##N_CLASS(CLASS_TO_STR(N_CLASS), parent); \
	gClass->SetClassFlags(classFlag); \
	PushGenericObject(gClass);\
	return gClass; \
	} \
	GODZ::GenericClass* N_CLASS::GetRuntimeClass() \
	{ \
	return PrivateRuntime##N_CLASS::_Class; \
	} \
	//__declspec(dllexport) const GenericClass* Local##N_CLASS = N_CLASS::GetClass(); \

//Declare this macro within the .cpp file to implement a GenericObject subclass declaration
//(otherwise you'll get linker errors)
#define ImplementGeneric(N_CLASS) \
	ImplementGenericFlags(N_CLASS, 0) \

#define ImplementAbstractClass(N_CLASS) \
	ImplementGenericFlags(N_CLASS, ClassFlag_Abstract) \

	//Forward Declarations
	class IGenericConfig;

	//object identifier
	typedef size_t ObjectID;
	static const int INVALID_ID = 0;


	class GODZ_API GenericObject
	{
		DeclareGenericRoot(GenericObject)

	public:
		virtual ~GenericObject();

		//Returns a unique clone of this object. The object is populated
		//with a copy of this objects serializable properties.
		virtual atomic_ptr<GenericObject> GetClone();
		void CopyTo(GenericObject* dest);

		//Returns the hash code for this object
		HashCodeID GetHashCode() { return m_HashCode; }
		void SetName(HString hash);
		HString GetName();

		// Tests to determine if this object is derived from the metaclass.
		bool IsA(GenericClass* Type);
		bool IsA(HString name);

		// Returns true if this is a dynamic object. If false is returned,
		// then this is a static class instance.
		virtual bool IsAObject();

		//Notification (editor mode) this entity has possibly been updated.
		virtual void OnPropertiesUpdated();

		//Notification this object has completed loading
		virtual void OnPostLoad();

		virtual void Serialize(GDZArchive& archive);

		// Garbage Collection - Final Cleanup
		static void DestroyAllObjects();

		// Coveince function that can be used to generate a new GUID
		static GUID CreateGUID();
		static GUID GetEmptyGUID();

		// Does a slow check to see if this object has a valid unique identifier. Should only be
		// used by Tools to save objects, etc
		bool GetGUIDFromPackage(GUID & id);

		//Returns the package this object belongs to. Note: this function
		//should only be invoked by the Editor, etc. Does O(N) searches all
		//over the place!
		virtual GenericPackage* GetPackage();

	protected:

		HString				m_HashCode;			//hash code for this object

		//Registers the object instance to the GenericObject::AllObjects vector
		GenericObject();

	};

	//Returns the current class index (indicates number of classes)
	GODZ_API size_t GetClassCount();

template <class T>
T* SafeCast(GODZ::GenericObject* obj)
{
	if (obj == NULL || !obj->IsA(T::GetClass()))
		return NULL;

	return static_cast<T*>(obj);
}

}
#include "IGenericConfig.h"

#endif //_GenericObject_H_
