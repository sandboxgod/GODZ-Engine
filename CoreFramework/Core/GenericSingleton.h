/* ===========================================================
	Singleton pattern based on techniques discussed in:
	Real Time 3D Terrain Engines Using C++ and DirectX

	Created Jan 24, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(GENERIC_SINGLETON_H)
#define GENERIC_SINGLETON_H

#include "Godz.h"
#include "GenericReferenceCounter.h"

namespace GODZ
{

	//Provides auto garbage collection capabilities to singletons.
	class GODZ_API SingletonClass
	{
	public:
		SingletonClass();
		virtual ~SingletonClass();

	protected:
		unsigned long GetThreadId() { return m_threadId; }

		//Return true if this singleton handles multiple thread calls concurrently
		virtual bool IsThreadSafe();

	private:
		unsigned long	m_threadId;		//my thread id for debugging
	};

	GODZ_API void PushSingleton(SingletonClass* pSingleton);
	void DestroySingletons();


	//Singleton metaclass provides base singleton functionality.
	template<class T>
	class GenericSingleton : public SingletonClass
	{
	public:

	    GenericSingleton(T& rObject)
		{
			//TODO: Display error only one instance of this class is allowed.	
			godzassert(!SingletonInstance);
	        SingletonInstance = &rObject;
		}

		static T* GetInstance()
		{
			//If this godzassert fires, that means we tried to get a singleton after shutdown. That would
			//be bad....
			godzassert(EngineStatus::engineRunning);

			if (SingletonInstance==NULL)
			{
				SingletonInstance = new T();
				PushSingleton(SingletonInstance);
			}

			//Failing this godzassert means a different thread has attempted to access this singleton. 
			//Singletons should only be accessed from the thread that created them
			godzassert(SingletonInstance->IsThreadSafe());
			return SingletonInstance;
		}

		static bool IsInstanced()
		{
			return SingletonInstance != NULL;
		}

	private:
		static T* SingletonInstance;
	};
	template <class T> T* GenericSingleton<T>::SingletonInstance = 0;
}

#endif //GENERIC_SINGLETON_H