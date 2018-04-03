
#include "StringTokenizer.h"

using namespace GODZ;
using namespace std;

StringTokenizer::StringTokenizer(const char* text, const char *seps)
: ptr(0)
{
	size_t len = strlen(text);
	char* buf = new char[len+1];
	StringCopy(buf,text,len+1);

	char* next_token = NULL;

	//Yes, Microsoft did the intelligent thing; they made functions such as strtok() be thread-safe by maintaining per-thread strtok() state.
	char* token = strtok_s( buf, seps, &next_token );
	while(token!=NULL)
	{
		strings.Push(token);
		token = strtok_s( NULL, seps, &next_token );
	}

	delete[] buf;

	strings.BuildArray();
}

const char* StringTokenizer::next()
{
	if (ptr<strings.GetNumItems())
	{
		ptr++;
		return strings[ptr-1];
	}

	return NULL;
}

size_t StringTokenizer::size()
{
	return strings.GetNumItems();
}

rstring StringTokenizer::operator[](size_t index)
{
	if (index >= strings.GetNumItems() )
	{
		rstring nullString;
		return nullString;
	}
	return strings[index];
}