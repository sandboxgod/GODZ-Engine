/* ===========================================================
	Reference Counted String Interface

	Created Nov 23,'09 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#include "StringBuffer.h"

#if !defined(_RSTRING_H_)
#define _RSTRING_H_

namespace GODZ
{
	// Reference Counted String
	class GODZ_API rstring : private GenericReference<StringBuffer>
	{
	public:
		rstring();
		rstring(StringBuffer* ref);
		rstring(const char* cdata);

		const char* c_str() const { return *this; }

		bool isEmpty() { return m_ref == NULL; }

		bool operator==(const char* text) const;
		bool operator==(const rstring& text) const;

		rstring& operator=(const char* cData);

		//Returns the internal char buffer
		operator const char*() const;

		//Adds another string to this one.
		rstring& operator+=(const char* cData);

		//Adds another string to this one.
		rstring& operator+(const char* cData);
	};
}

#endif