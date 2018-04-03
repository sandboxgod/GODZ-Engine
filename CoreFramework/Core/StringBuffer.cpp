
#include "StringBuffer.h"
#include "Godz.h"

using namespace GODZ;

StringBuffer::StringBuffer()
:c_Buffer(NULL)
{
}

StringBuffer::StringBuffer(const char*value)
{
	if (value == NULL)
	{
		free(c_Buffer);
		c_Buffer=NULL;
		return;
	}

	size_t size = strlen(value) + 1;
	c_Buffer = static_cast<char*>(malloc(size * sizeof(char)));
	StringCopy(c_Buffer, value, size);
}

StringBuffer::~StringBuffer()
{
	if (c_Buffer != NULL)
	{
		//Log("freeing %s %d\n", c_Buffer,strlen(c_Buffer));
		free(c_Buffer);	
		c_Buffer=NULL;
	}	
}

void StringBuffer::AppendData(const char* cData)
{
	if (c_Buffer != NULL)
	{
		size_t len = strlen(cData);
		size_t my_size = strlen(c_Buffer);
		size_t newsize = (my_size+len+1);
		c_Buffer = static_cast<char*>(realloc(c_Buffer, newsize));
		strcat_s(c_Buffer, newsize, cData);
	}
	else
	{
		SetData(cData); // initialize the buffer
	}
}

const char* StringBuffer::c_str() const
{
	if (c_Buffer != NULL)
	{
		return c_Buffer;
	}

	return EMPTY_STRING;
}

void StringBuffer::SetData(const char* cData)
{
	if (c_Buffer)
	{
		free(c_Buffer); //release the old buffer
		c_Buffer=NULL;
	}

	if (cData == NULL)
		return;

	//reassigns the char buffer.
	size_t size=strlen(cData)+1;
	c_Buffer=static_cast<char*>(malloc(size));
	godzassert(c_Buffer!=NULL); //,"Out of memory: Cannot allocate char buffer.");
	StringCopy(c_Buffer,cData, size);
}

size_t StringBuffer::StaticFindSubstring(const char* cData, const char* substr)
{
	size_t len = strlen(cData);
	size_t substr_len=strlen(substr);
	size_t index=0;
	size_t start_index=0;

	bool bFound=false;
	for (size_t i = 0; i < len;i++)
	{
		if (cData[i] == substr[index])
		{
			if (!bFound)
			{
				bFound=true;
				start_index=i;
			}

			index++; //compare next loc
			if (index == substr_len) //found the substring?
			{
				return start_index;
			}
		}
		else if (bFound)
		{
			//reset the search
			index=0;
			bFound=0;
		}
	}

	return -1;
}

StringBuffer& StringBuffer::operator=(const char* cData)
{
	SetData(cData); //use inlined functions to avoid 'localized heap' exceptions
	return *this;
}

StringBuffer::operator const char*() const
{
	if (c_Buffer != NULL)
	{
		return c_Buffer;
	}

	return EMPTY_STRING;
}

StringBuffer& StringBuffer::operator+(const char* cData)
{
	AppendData(cData); //use inlined functions to avoid 'localized heap' exceptions
	return *this;
}

StringBuffer& StringBuffer::operator+=(const char* cData)
{
	AppendData(cData); //use inlined functions to avoid 'localized heap' exceptions
	return *this;
}
