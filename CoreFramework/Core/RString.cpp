
#include "RString.h"
#include "Godz.h"

using namespace GODZ;

rstring::rstring()
{
	m_ref = NULL;
}

rstring::rstring(StringBuffer* ref)
{
	m_ref = ref;
	m_ref->IncrementReferences();
}

rstring::rstring(const char* cdata)
{
	if (cdata != NULL)
	{
		m_ref = new StringBuffer();
		m_ref->IncrementReferences();
		((StringBuffer*)m_ref)->SetData(cdata);
	}
	else
	{
		m_ref=NULL;
	}
}

bool rstring::operator==(const char* text) const
{
	const char* t1 = *this;

	if (t1 == NULL && text == NULL)
		return true;

	// If either string doesn't have a value then they don't equal
	if (t1 == NULL || text == NULL)
		return false;

	return strcmp(t1,text) == 0;
}

bool rstring::operator==(const rstring& text) const
{
	const char* t1 = *this;
	const char* t2 = *text;

	if (t1 == NULL && t2 == NULL)
		return true;

	// If either string doesn't have a value then they don't equal
	if (t1 == NULL || t2 == NULL)
		return false;

	return strcmp(t1,t2) == 0;
}

rstring& rstring::operator=(const char* cData)
{
	if (cData == NULL)
	{
		if (m_ref)
		{
			m_ref->DecrementReferences();
			m_ref = NULL;
		}

		return *this;
	}

	if (m_ref == NULL)
	{
		m_ref = new StringBuffer();
		m_ref->IncrementReferences();
	}

	((StringBuffer*)m_ref)->SetData(cData);
	return *this;
}

rstring::operator const char*() const
{
	if (m_ref != NULL)
	{
		return ((StringBuffer*)m_ref)->c_str();
	}

	return EMPTY_STRING;
}

//Adds another string to this one.
rstring& rstring::operator+=(const char* cData)
{
	if (!m_ref)
	{
		m_ref = new StringBuffer();
		m_ref->IncrementReferences();
	}

	if(cData!=NULL)
	{
		((StringBuffer*)m_ref)->AppendData(cData);
	}

	return *this;
}

//Adds another string to this one.
rstring& rstring::operator+(const char* cData)
{
	if (!m_ref)
	{
		m_ref = new StringBuffer();
		m_ref->IncrementReferences();
	}

	((StringBuffer*)m_ref)->AppendData(cData);
	return *this;
}