/* ===========================================================
	GodzThread

	Basic Implementation that hides the hardware specific
	implementation of the thread

	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__GodzAtomics_H__)
#define __GodzAtomics_H__

#include "CoreFramework.h"
#include "GenericReferenceCounter.h"
#include "GodzAssert.h"

namespace GODZ
{
	long AtomicIncrement(volatile long& var);

	//The function returns the resulting decremented value.
	long AtomicDecrement(volatile long& var);
	void AtomicSet(volatile long& var, long newValue);

	/*
	* Lock-free programming method to access an atomic variable
	*/
	class GODZ_API GodzAtomic
	{
	public:
		GodzAtomic();

		//returns true if this atomic variable can be locked . Returns false if it has
		//already been locked
		bool Set();

		//Returns true if this atomic has been set
		bool IsSet() const;

		void UnSet();

	protected:
		//non-copying
		GodzAtomic(const GodzAtomic& other);

		long m_value;
	};

	/*
	* Futures stores a reference to an object that is expected to be available sometime in the future...
	*/
	template <class T>
	class Future
	{

	public:
		Future()
		{
			m_future = new FutureObject<T>();
		}

		bool isReady()
		{
			return m_future->isReady();
		}

		T getValue()
		{
			return m_future->getValue();
		}

		void setValue(T value)
		{
			m_future->setValue(value);
		}

		// comparison tests
		bool operator==(Future<T>& obj) const
		{
			return m_future == obj.m_future;
		}

		bool operator!=(Future<T>& obj) const
		{
			return m_future != obj.m_future;
		}


	private:
		//Inner class
		template <class T>
		class FutureObject : public GenericReferenceCounter
		{

		public:
			bool isReady()
			{
				return m_isReady.IsSet();
			}

			T getValue()
			{
				godzassert(m_isReady.IsSet());
				return value;
			}

			void setValue(T value)
			{
				this->value = value;
				m_isReady.Set();
			}

		private:
			GodzAtomic m_isReady;
			T value;
		};

		GenericReference<FutureObject<T> > m_future;
	};

	/*
	* Can be used to synchronize a block of code....
	*/
	class SynchronizedBlock
	{
	public:
		SynchronizedBlock(GodzAtomic& atom);
		~SynchronizedBlock();

	private:
		GodzAtomic& m_lock;
	};

	/*
	* The traditional thread-safe int class
	*/
	class GODZ_API AtomicInt
	{
	public:
		AtomicInt();
		AtomicInt(long temp);
		AtomicInt& operator=(long other);
		operator long() const;

		//postfix operators
		AtomicInt operator++(int);
		AtomicInt operator--(int);

		//prefix operators
		AtomicInt& operator++();
		AtomicInt& operator--();

	private:
		volatile long m_value;
	};

	class GODZ_API AtomicBool
	{
	public:
		AtomicBool();
		AtomicBool(bool temp);
		AtomicBool& operator=(bool value);
		operator bool() const;

	private:
		volatile long m_value;
	};

	/*
	* Synchronized variable template.
	*/
	template <class T>
	class SyncPoint
	{
	public:
		SyncPoint()
		{
		}

		SyncPoint(const T& temp)
			: m_value(temp)
		{
		}

		void get(T& temp) const
		{
			while (!m_lock.Set());
			temp = m_value;
			m_lock.UnSet();
		}

		void set(const T& temp)
		{
			while (!m_lock.Set());
			m_value = temp;
			m_lock.UnSet();
		}

		bool isBusy()
		{
			return m_lock.IsSet();
		}

		SyncPoint& operator=(const T& other)
		{
			while (!m_lock.Set());
			m_value = other;
			m_lock.UnSet();
			return *this;
		}

		operator T() const
		{
			T result;

			while (!m_lock.Set());
			result = m_value;
			m_lock.UnSet();

			return result;
		}


		// comparison tests
		bool operator==(const T& obj) const
		{
			bool result;

			while (!m_lock.Set());
			result = (m_value == obj);
			m_lock.UnSet();

			return result;
		}

		bool operator!=(T& obj) const
		{
			bool result;

			while (!m_lock.Set());
			result = (m_value != obj);
			m_lock.UnSet();

			return result;
		}

	private:
		//marked mutable since we always have to lock/unlock even within const functions
		mutable GodzAtomic m_lock;
		T m_value;
	};

	/*
	* Less prone to lock contention then SyncPoint<T> at the cost of additional memory use and slightly more complexity. Callers should probably
	* just stick to a max of 2 (Max_Size == 2) for double buffered values
	*/
	template <class T, unsigned int Max_Size>
	class RingBuffer
	{
	public:

		RingBuffer()
			: m_index(0)
		{
			for(long i = 0; i < Max_Size; i++)
			{
				m_isInit[i] = false;
			}
		}

		//RingBuffers must be initialized with some default value. This way we can be certain garbage isn't returned
		RingBuffer(const T& temp)
			: m_index(0)
		{
			for(long i = 0; i < Max_Size; i++)
			{
				m_list[i] = temp;
				m_isInit[i] = true;
			}
		}

		void set(const T& temp)
		{
			//loop until you find an empty slot
			while(;;)
			{
				for(long i = 0; i < Max_Size; i++)
				{
					if (m_lock[i].Set())
					{
						m_list[i] = temp;
						m_index = i;
						m_lock[i].UnSet();
						m_isInit[i] = true;
						return;
					}
				}
			}
		}

		void get(T& temp) const
		{
			if (m_lock[m_index].Set())
			{
				temp = m_list[m_index];
				m_lock[m_index].UnSet();
				return;
			}

			while(;;)
			{
				for(long i = 0; i < Max_Size; i++)
				{
					if (m_isInit[i] == true && m_lock[i].Set())
					{
						temp = m_list[i];
						m_lock[i].UnSet();
						return;
					}
				}
			}
		}

		RingBuffer& operator=(const T& other)
		{
			set(other);
			return *this;
		}

		operator T() const
		{
			T result;
			get(result);

			return result;
		}

	private:
		AtomicInt m_index;

		T m_list[Max_Size];
		AtomicBool m_isInit[Max_Size];			//set to 1 if this slot is initialized with a value
		mutable GodzAtomic m_lock[Max_Size];
	};

}

#endif //__GodzAtomics_H__