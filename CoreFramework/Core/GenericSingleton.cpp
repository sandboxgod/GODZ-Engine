
#include "GenericSingleton.h"
#include <windows.h>

using namespace GODZ;
using namespace std;

//singletons are stored inside a vector. when the engine shutsdown, all singletons
//are destroyed
vector<SingletonClass*> SingletonIterator;

SingletonClass::SingletonClass()
: m_threadId(0)
{
	//buffer my current thread id which we use for tracking thread access violations
	m_threadId = GetCurrentThreadId();
}

SingletonClass::~SingletonClass()
{
}

bool SingletonClass::IsThreadSafe()
{
	//if we are thread safe always return true....
	return GetCurrentThreadId() == m_threadId;
}


GODZ_API void GODZ::PushSingleton(SingletonClass* pSingleton)
{
	SingletonIterator.push_back(pSingleton);
}

void GODZ::DestroySingletons()
{
	size_t size = SingletonIterator.size();
	for(udword i=0;i<size;i++)
	{
		delete SingletonIterator[i];
	}
}

