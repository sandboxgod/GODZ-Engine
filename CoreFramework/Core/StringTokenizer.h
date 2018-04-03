/* ===========================================================
	String Tokenizer

	Created Apr 27, '04 by Richard Osborne
	Copyright (c) 20010
	========================================================== 
*/

#if !defined(_STRING_TOKENIZER_H_)
#define _STRING_TOKENIZER_H_

#include "Godz.h"
#include "GenericNode.h"

namespace GODZ
{
	class GODZ_API StringTokenizer
	{
	public:
		StringTokenizer(const char* text, const char* seps);
		const char* next();
		size_t size();
		rstring operator[](size_t index);

	private:
		NodeList<rstring> strings;
		UInt32 ptr;
	};
}

#endif