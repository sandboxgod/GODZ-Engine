/* ==============================================================
	Class: GDZArchive

	Created July 12, '04 by Richard Osborne
	Copyright (c) 2010
	=============================================================
*/

#if !defined(_GDZARCHIVE_H_)
#define _GDZARCHIVE_H_

#include "CoreFramework.h"
#include "RString.h"
#include <guiddef.h>

namespace GODZ
{
	//forward decl
	class IReader;
	class IWriter;
	class ResourceObject;
	class TextureResource;
	class GenericObject;

	// current version of godz packages
	static const int GODZ_SAVE_VERSION = 1010;

	class GODZ_API GDZArchive
	{
	public:
		GDZArchive(IReader* reader);
		GDZArchive(IWriter* writer);
		void Close();
		bool IsSaving();

		IWriter* GetWriter() { return m_pWriter; }
		IReader* GetReader() { return m_pReader; }
		int GetFileVersion();
		void SetFileVersion(int ver) { m_nFileVersion = ver; }

		//serializes an array of elements (does fread/fwrite)
		size_t Serialize(void* ptr, size_t dataTypeSize, size_t numElements);

		GDZArchive& operator<<(long& value);
		GDZArchive& operator<<(bool& value);
		GDZArchive& operator<<(int& i);
		GDZArchive& operator<<(float& f);

		GDZArchive& operator<<(udword& word);
		GDZArchive& operator<<(unsigned long& word);
		GDZArchive& operator<<(u8& ch);
		GDZArchive& operator<<(UInt16& us);
		GDZArchive& operator<<(rstring& s);
		GDZArchive& operator<<(GenericObject*& genericobj);
		GDZArchive& operator<<(ResourceObject*& resource);
		GDZArchive& operator<<(GUID& uid);
		

	protected:
		IReader* m_pReader;
		IWriter* m_pWriter;
		bool m_bIsSaving;
		int m_nFileVersion;	//file version
	};

	//saves the Version Information
	struct GODZ_API ArchiveVersion
	{
		int m_nFirstVersion;			//This is the first valid version we can load
		int m_nCurrentVersion;			//Current Version of the saved data

		ArchiveVersion(int firstVersion, int currentVersion)
			: m_nFirstVersion(firstVersion)
			, m_nCurrentVersion(currentVersion)
		{

		}

		friend GDZArchive& operator<<(GDZArchive& ar, ArchiveVersion& version) 
		{
			ar << version.m_nFirstVersion << version.m_nCurrentVersion;
			return ar;
		}
	};
}

#endif