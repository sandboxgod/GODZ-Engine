
#include "Name.h"
#include "Godz.h"
#include <tchar.h>

using namespace GODZ;

FName::FName()
{
	m_buf=0;
	this->size=0;
}

FName::FName(const FName& other)
{
	const char* text = other;
	size = strlen(text) + 1;
	m_buf = new char[size];
	StringCopy(m_buf,text, size);
}

FName::FName(const char* text)
{
	size = strlen(text) + 1;
	m_buf = new char[size];
	StringCopy(m_buf, text, size);
}

FName::FName(size_t size)
{
	m_buf = new char[size];
	this->size=size;
	
}

FName::~FName()
{
	delete[] m_buf;
	m_buf=NULL;
}


FName& FName::operator=(const char* pText)
{
	if (strlen(pText) > size)
	{
		delete m_buf;
		size = strlen(pText) + 1;
		m_buf = new char[size];
	}

	StringCopy(m_buf,pText, size);

	return *this;
}

FName::operator const char*() const
{
	return m_buf;
}

FName& FName::operator+=(const char* text)
{
	return *this;
}
