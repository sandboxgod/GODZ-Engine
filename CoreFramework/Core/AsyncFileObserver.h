/* ===========================================================
	AsyncFileObserver

	Created Mar 9, '08 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__ASYNCFILEOBSERVER_H__)
#define __ASYNCFILEOBSERVER_H__

#include "Godz.h"

namespace GODZ
{

	/**
	*	Observer waiting on a file job to complete (main thread call). Not valid
	*	from other threads of course.
	*/
	class GODZ_API AsyncFileObserver
	{
	public:
		virtual void OnJobCompleted() = 0;
		virtual ~AsyncFileObserver();
	};

}

#endif //__ASYNCFILEOBSERVER_H__
