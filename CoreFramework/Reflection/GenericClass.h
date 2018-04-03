/* =================================================================

	===============================================================
*/

#if !defined(_GenericClass_H_)
#define _GenericClass_H_

#include <CoreFramework/Core/GenericObject.h>
#include <CoreFramework/Core/atomic_ref.h>
#include <CoreFramework/Core/Material.h>
#include <hash_map>
#include <vector>

namespace GODZ
{
	class ClassProperty;

	// Used to identify objects (Represents an object type)
	class GODZ_API GenericClass: public GenericObject
	{
	public:
		// Creates a Static Class Type
		GenericClass(const char* Name=NULL, GenericClass* parent=NULL);
		~GenericClass();

		// Returns the runtime class definition
		virtual GODZ::GenericClass* GetRuntimeClass();

		// Returns the flags for this class.
		int GetClassFlags();
		void SetClassFlags(int flags) { ClassFlags = flags; }

		// Creates an object represented by this static class.
		virtual GenericObject* GetInstance(const char* ObjectName=NULL, GenericClass* parent=NULL);

		// Returns the static class type of the 'parent' of this class
		GenericClass* GetSuperClass();
		bool IsParent(GenericClass* godzClass);

		void AddProperty(ClassProperty* property);
		ClassProperty* GetProperty(HString propertyName);
		size_t GetNumProperties();
		ClassProperty* GetPropertyAt(HashCodeID hash);
		void GetProperties(std::vector<ClassProperty*>& list);
		void GetPropertiesNonRecurse(std::vector<ClassProperty*>& list);

		virtual bool IsAObject() { return false; }

		//Returns true if objects of this type can be added to the World directly as an interactable object
		bool IsPlaceable();

		const char* GetName() { return mName; }

		atomic_ptr<Material> GetMaterial();
		void SetMaterial(atomic_ptr<Material> m);

		// Invoked to notify this object of the shared ptr that points to it
		void SetReference(const atomic_ptr<GenericClass>& ptr);
		atomic_ptr<GenericClass> GetReference();

		GenericPackage* GetPackage() { return m_package; }

	protected:
		//Initialization
		void Setup(const char* Name=NULL, GenericClass* parent=NULL);

		GenericClass* Super; // pointer to the parent class.
		int ClassFlags; //Class Attributes (see EGenericFlags)

		typedef stdext::hash_map<HashCodeID, ClassProperty* > PropertyList;
		PropertyList				m_propertyList;

		rstring						mName;	//for debugging

		//Sprite used by the Editor
		atomic_ptr<Material>	m_mat;

		GenericPackage* m_package;
	};
}

#endif //
