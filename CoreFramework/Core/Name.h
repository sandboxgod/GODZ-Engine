/* ===========================================================
	Name.h

	Created Nov 3,'04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(__NAME_H__)
#define __NAME_H__

#include "CoreFramework.h"

namespace GODZ
{
	class GODZ_API FName
	{
	public:
		FName();
		FName(const char* text);
		FName(const FName&);
		FName(size_t size);
		~FName();
		FName& operator=(const char* pText);
		//FName& operator=(const FName&);
		operator const char*() const;
		FName& operator+=(const char* text);

	private:
		size_t size;
		char *m_buf;
	};
}

#endif