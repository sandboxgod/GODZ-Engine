/* ===========================================================
	Defines a simple, shared atomic_ptr

	Copyright (c) 2012
	========================================================== 
*/

#if !defined(__ATOMICREF_H)
#define __ATOMICREF_H

// Turns on tracking a thread safe list that tracks all the T*.
/*
#if !defined(ATOMIC_REF_DEBUGGING) && defined(_DEBUG)
#define ATOMIC_REF_DEBUGGING 1
#endif
*/

#include "GodzAtomics.h"
#include "GodzAssert.h"
#include <hash_map>

namespace GODZ
{
	template <class T> class atomic_weak_ptr;

	struct StackInfo
	{
		//TODO: Save callstack...
	};

	template <class T>
	class AtomicCount
	{
	public:
		AtomicCount()
			: ptr(NULL)
		{
		}

		AtomicCount(T* p )
			: ptr(p)
		{

#if defined(ATOMIC_REF_DEBUGGING)
			{
				// block until lock is free
				while(m_debugMapLock > 0);
				++m_debugMapLock;
				// Make sure no other AtomicCount owns this
				godzassert(m_debugPtrMap.find(p) == m_debugPtrMap.end());
				StackInfo info;
				m_debugPtrMap[p] = info;
				--m_debugMapLock;
			}
#endif
		}

		~AtomicCount()
		{
			godzassert(m_count == 0 && m_weakCount == 0);
		}

		void AddShared()
		{
			// It's faster to invoke pre-increment on AtomicInts
			++m_count;
		}

		void SubtractShared()
		{
			godzassert(m_count > 0);

			// It's faster to invoke pre-increment on AtomicInts
			--m_count;
			if (m_count == 0 && ptr != NULL)
			{

#if defined(ATOMIC_REF_DEBUGGING)
				{
					// block until lock is free
					while(m_debugMapLock > 0);
					++m_debugMapLock;

					DebugMap::iterator iter = m_debugPtrMap.find(ptr);
					if (iter != m_debugPtrMap.end())
					{
						m_debugPtrMap.erase(iter);
					}
					--m_debugMapLock;
				}
#endif

				// Free the allocated memory
				delete ptr;
				ptr = NULL;
			}

			if (m_count == 0 && m_weakCount == 0)
			{
				delete this;
			}
		}

		long GetShared() const
		{
			return m_count;
		}

		void AddWeak()
		{
			++m_weakCount;
		}

		void SubtractWeak()
		{
			godzassert(m_weakCount > 0);
			--m_weakCount;
			if( m_weakCount == 0 && m_count == 0)
			{
				delete this;
			}
		}

		long GetWeak() const
		{
			return m_weakCount;
		}

		T* GetPtr()
		{
			return ptr;
		}

	private:
		AtomicInt m_count;
		AtomicInt m_weakCount;
		T* ptr;

		typedef stdext::hash_map<T*, StackInfo> DebugMap;
		static DebugMap m_debugPtrMap;
		static AtomicInt m_debugMapLock;
	};

	template <class T>
	AtomicInt AtomicCount<T>::m_debugMapLock;

	template <class T>
	stdext::hash_map<T*, StackInfo> AtomicCount<T>::m_debugPtrMap;

	/*
	* Sharable, Smart pointer
	*/
	template<class T>
	class atomic_ptr
	{
		friend atomic_weak_ptr<T>;
		template< class T, class T2 > friend atomic_ptr<T> atomic_ptr_cast( atomic_ptr<T2>& pointer );

	public:
		atomic_ptr()
		{
			// Always keep an atom allocated so operators wont crash
			m_atom = new AtomicCount<T>();
			m_atom->AddShared();
		}

		explicit atomic_ptr(T* p)
		{
			m_atom = new AtomicCount<T>(p);
			m_atom->AddShared();
		}

		// Copy constructor
		atomic_ptr(const atomic_ptr& other)
		{
			m_atom = other.m_atom;
			m_atom->AddShared();
		}

		explicit atomic_ptr(AtomicCount<T>* newAtom)
			: m_atom(newAtom)
		{
			m_atom->AddShared();
		}

		// Conversion constructor
		template<class T2> atomic_ptr( const atomic_ptr<T2>& pointer )
		{
			m_atom = (AtomicCount<T>*)( pointer.m_atom );

			if (m_atom != NULL)
			{
				m_atom->AddShared();
			}
		}

		~atomic_ptr()
		{
			m_atom->SubtractShared();
			m_atom = NULL;
		}

		//assignment
		template<class T2> inline atomic_ptr<T>& operator = ( const atomic_ptr<T2>& pointer )
		{
			if (m_atom != pointer.m_atom)
			{
				if (m_atom != NULL)
				{
					m_atom->SubtractShared();
				}
				m_atom = (AtomicCount<T>*)( pointer.m_atom );

				if (m_atom != NULL)
				{
					m_atom->AddShared();
				}
			}

			return *this;
		}

		atomic_ptr& operator=(const atomic_ptr& other)
		{
			if (m_atom != other.m_atom)
			{
				m_atom->SubtractShared();

				m_atom = other.m_atom;
				m_atom->AddShared();
			}

			return *this;
		}

		atomic_ptr& operator=(T* obj)
		{
			if (obj != m_atom->GetPtr())
			{
				m_atom->SubtractShared();
				if( obj != NULL )
				{
					m_atom = new AtomicCount<T>(obj);
					m_atom->AddShared();
				}
				else
				{
					// Always keep an atom around so operators wont crash
					m_atom = new AtomicCount<T>();
					m_atom->AddShared();
				}
			}

			return *this;
		}

		long use_count() const
		{
			return m_atom->GetShared();
		}


		//conversions
		operator T*() const 
		{
			return (T*)m_atom->GetPtr(); 
		}
		
		T& operator*() const 
		{
			return *(((T*)m_atom->GetPtr())); 
		}

		T* operator->() const
		{
			return (T*)m_atom->GetPtr();
		}

		bool operator==(T* obj) const
		{
			return ((T*)m_atom->GetPtr()) == obj;
		}

		bool operator!=(T* obj) const
		{
			return ((T*)m_atom->GetPtr()) != obj;
		}

		AtomicCount<T>* m_atom;
	};

	/*
	* Weak reference (or rather handle) to an object.
	*/
	template <class T>
	class atomic_weak_ptr
	{
	public:
		atomic_weak_ptr()
			: m_atom(NULL)
		{
		}

		atomic_weak_ptr(atomic_ptr<T>& p)
		{
			m_atom = p.m_atom;
			if( m_atom != NULL )
			{
				m_atom->AddWeak();
			}
		}

		atomic_weak_ptr(const atomic_weak_ptr<T>& p)
		{
			m_atom = p.m_atom;
			if( m_atom != NULL )
			{
				m_atom->AddWeak();
			}
		}

		~atomic_weak_ptr()
		{
			if( m_atom != NULL)
			{
				m_atom->SubtractWeak();
				m_atom = NULL;
			}
		}

		atomic_weak_ptr& operator=(const atomic_ptr<T>& p)
		{
			if( m_atom != NULL)
			{
				m_atom->SubtractWeak();
			}

			m_atom = p.m_atom;
			if( m_atom != NULL )
			{
				m_atom->AddWeak();
			}

			return *this;
		}

		atomic_weak_ptr& operator=(const atomic_weak_ptr<T>& p)
		{
			if( m_atom != NULL)
			{
				m_atom->SubtractWeak();
			}

			m_atom = p.m_atom;
			if( m_atom != NULL )
			{
				m_atom->AddWeak();
			}

			return *this;
		}

		atomic_ptr<T> lock()
		{
			if( m_atom != NULL )
				return atomic_ptr<T>(m_atom);

			return atomic_ptr<T>();
		}

		void reset()
		{
			if( m_atom != NULL)
			{
				m_atom->SubtractWeak();
				m_atom = NULL;
			}
		}

		AtomicCount<T>* m_atom;
	};

	template< class T, class T2 >
	atomic_ptr<T> atomic_ptr_cast( atomic_ptr<T2>& pointer )
	{
		AtomicCount<T>* count = (AtomicCount<T>*)( pointer.m_atom );
		return atomic_ptr<T>( count );
	}

	template< class T, class T2 >
	atomic_weak_ptr<T> atomic_ptr_cast( atomic_weak_ptr<T2>& pointer )
	{
		AtomicCount<T>* count = (AtomicCount<T>*)( pointer.m_atom );
		return atomic_ptr<T>( count );
	}
}

#endif