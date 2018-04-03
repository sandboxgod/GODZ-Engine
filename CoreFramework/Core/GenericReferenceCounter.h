/* ===========================================================
	Defines Reference Counting Utility Classes

	Applies a smart pointer as discussed in 
	3D Game Engine Design by David Eberly.

	Updated for thread safety on Apr 30, '07

	Created Jan 24, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__GENERICREFERENCECOUNTER_H)
#define __GENERICREFERENCECOUNTER_H

#include "CoreFramework.h"

namespace GODZ
{
	//Base interface class for objects that store reference counts. Used for debugging
	class GODZ_API IReferenceCountObserver
	{
	public:

		//Returns the name of this observer; default name is unknown
		virtual const char* GetObserverName() { return "Unknown"; }
	};

	//Base Reference Counter
	class GODZ_API GenericReferenceCounter
	{
	public:
		GenericReferenceCounter();
		virtual ~GenericReferenceCounter();

		long			GetNumReferences();
		void			IncrementReferences();
		void			DecrementReferences();

		//Should only be called if this object was created on the stack
		void			DecrementStackReferences();

		//advanced function to adjust reference count. needed if you indirectly clone
		//the object and get back a false reference count
		void			ResetReferrers(long value=1);

		//blocks copying other the other objects reference count
		GenericReferenceCounter(const GenericReferenceCounter& counter);

	private:
		long mCount;
	};




	//Smart Pointer Implementation
	template<class T> class GenericReference
	{
	public:

		GenericReference()
			: m_ref(NULL)
		{
		}

		GenericReference(T* ref)
			: m_ref(ref)
		{
			if (m_ref != NULL)
			{
				m_ref->IncrementReferences();
			}
		}

		template<class T2> GenericReference( const GenericReference<T2>& pointer )
		{
			m_ref = static_cast<T*>( static_cast< T2* >( pointer.m_ref ) );

			if (m_ref != NULL)
			{
				m_ref->IncrementReferences();
			}
		}

		GenericReference(const GenericReference& grPtr)
		{
			m_ref = grPtr.m_ref;

			if (m_ref != NULL)
			{
				m_ref->IncrementReferences();
			}
		}

		~GenericReference()
		{
			if (m_ref != NULL)
			{
				m_ref->DecrementReferences();
			}
		}

		//conversions
		operator T*() const 
		{
			return (T*)m_ref; 
		}
		
		T& operator*() const 
		{ 
			return *(((T*)m_ref)); 
		}

		T* operator->() const
		{
			return (T*)m_ref;
		}

		//assignment
		template<class T2> inline GenericReference<T>& operator = ( const GenericReference<T2>& pointer )
		{
			if (m_ref != pointer.m_ref)
			{
				if (m_ref != NULL)
				{
					m_ref->DecrementReferences();
				}
				m_ref = static_cast<T*>( static_cast< T2* >( pointer.m_ref ) );
				if (m_ref != NULL)
				{
					m_ref->IncrementReferences();
				}
			}

			return *this;
		}

		GenericReference& operator=(const GenericReference& gr_ptr)
		{
			if (m_ref != gr_ptr.m_ref)
			{
				if (m_ref != NULL)
				{
					m_ref->DecrementReferences();
				}
				m_ref = gr_ptr.m_ref;
				if (m_ref != NULL)
				{
					m_ref->IncrementReferences();
				}
			}

			return *this;
		}

		GenericReference& operator=(T *obj)
		{
			if (m_ref != obj)
			{
				if (m_ref != NULL)
				{
					m_ref->DecrementReferences();
				}

				m_ref = obj;

				if (m_ref != NULL)
				{
					m_ref->IncrementReferences();
				}
			}

			return *this;
		}

		// comparison tests
		bool operator==(GenericReference<T>& other) const
		{
			return m_ref == other.m_ref;
		}

		bool operator==(T* obj) const
		{
			return ((T*)m_ref) == obj;
		}

		bool operator!=(T* obj) const
		{
			return ((T*)m_ref) != obj;
		}

	public:
		//We do this instead of (T* m_ref) so that when a ref is declared we don't have to know what it is
		GenericReferenceCounter* m_ref;
	};
}

#endif //__GENERICREFERENCECOUNTER_H