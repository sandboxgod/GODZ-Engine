/* ===========================================================
	HString

	Created 8/24/08
	========================================================== 
*/

#if !defined(__HSTRING__H__)
#define __HSTRING__H__

#include "CoreFramework.h"
#include "RString.h"
#include "GDZArchive.h"

namespace GODZ
{

	/*
	*	Represents a hash string
	*/
	class GODZ_API HString
	{
	public:
		HString();
		HString(const char* text);
		HString(rstring& text);
		HString(HashCodeID hash);

		HashCodeID	getHashCode() const { return m_HashCode; }
		
		bool isEmpty() const { return m_HashCode == 0; }

		// Returns the encapsulated char string
		const char* c_str() const;
		void Serialize(GDZArchive& ar);

		bool operator==(const HString& hashString) const
		{
			return m_HashCode == hashString.m_HashCode;
		}

		bool operator==(HashCodeID id) const
		{
			return m_HashCode == id;
		}

		operator HashCodeID()
		{
			return m_HashCode;
		}

		friend GDZArchive& operator<<(GDZArchive& ar, HString& str)
		{
			str.Serialize(ar);
			return ar;
		}

	protected:
		HashCodeID		m_HashCode;			//hash code for this object

#ifdef _DEBUG
		rstring mText;	//for debugging
#endif _DEBUG

	};	
}

#endif //__HSTRING__H__
