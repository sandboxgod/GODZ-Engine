/* ===========================================================
	GodzMutex


	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__GODZMUTEX_H__)
#define __GODZMUTEX_H__

#include "Godz.h"

namespace GODZ
{
	class GODZ_API GodzMutex
	{
	public:
		GodzMutex(HANDLE mutex)
			: m_mutex(mutex)
		{
			WaitForSingleObject(m_mutex, INFINITE);
		}

		~GodzMutex()
		{
			ReleaseMutex(m_mutex);
		}

	private:
		HANDLE m_mutex;
	};


}

#endif //__GODZMUTEX_H__