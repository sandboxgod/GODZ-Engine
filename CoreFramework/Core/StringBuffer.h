/* ===========================================================
	Reference Counted String Interface

	Created Feb 1,'04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#include "GenericReferenceCounter.h"
#include <stdio.h>
#include <stdarg.h>

#if !defined(_STRING_BUFFER_H_)
#define _STRING_BUFFER_H_

namespace GODZ
{
	static const char EMPTY_STRING[1] = "";

	class GODZ_API StringBuffer : public GenericReferenceCounter
	{
	public:
		StringBuffer();
		StringBuffer(const char* value);
		virtual ~StringBuffer();

		void AppendData(const char* cData);

		//Returns the internal char buffer
		const char* c_str() const;

		void SetData(const char* cData);

		//Assigns a char buffer to this one.
		StringBuffer& operator=(const char* cData);

		//Returns the internal char buffer
		operator const char*() const;

		//Adds another string to this one.
		StringBuffer& operator+=(const char* cData);

		//Adds another string to this one.
		StringBuffer& operator+(const char* cData);

		// Returns the index of the substring. -1 is returned if the substring
		// isn't found.
		static size_t StaticFindSubstring(const char* cData, const char* substr);

	private:
		char* c_Buffer;
	};

	class StringVar
	{
	public:
		StringVar(char * format, ...)
		{
			va_list args; 
			va_start(args, format); 
			vsprintf_s(m_Buffer, sizeof(m_Buffer)/sizeof(char), format, args);
			va_end(args); 
		}

		const char* c_str()
		{
			return m_Buffer;
		}
	protected:
		char m_Buffer[2048];
	};
}

#endif