
#include "HString.h"
#include "crc32.h"
#include "HStringTable.h"

namespace GODZ
{
HString::HString()
	: m_HashCode(0)
{
}

HString::HString(const char* text)
{
	m_HashCode = ECRC32::Get_CRC(text);

#ifdef _DEBUG
	mText = text;
#endif
}

HString::HString(rstring& text)
{
	m_HashCode = ECRC32::Get_CRC(text.c_str());

#ifdef _DEBUG
	mText = text;
#endif
}

HString::HString(HashCodeID hash)
: m_HashCode(hash)
{
#ifdef _DEBUG
	//See if the hstring table has it
	mText = HStringTable::GetInstance()->GetString( hash );
#endif
}

const char* HString::c_str() const
{
#ifdef _DEBUG
	return mText;	//for debugging
#else
	return "";		//empty string in release...
#endif
}

void HString::Serialize(GDZArchive& ar)
{
	ar << m_HashCode;

#ifdef _DEBUG
	//See if the hstring table has it
	mText = HStringTable::GetInstance()->GetString( m_HashCode );
#endif
}

} //namespace GODZ
