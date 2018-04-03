//---------------------------------------------------------------------------
#ifndef crc32H
#define crc32H
#include <windows.h>
#include "godz.h"
//---------------------------------------------------------------------------

class GODZ_API ECRC32 {
public:
        // NO CONSTRUCTOR OR DESTRUCTOR NEEDED HERE
        static ULONG	Get_CRC(const char* text) ;
        static DWORD    CRC32ByStruct( BYTE* byStruct, DWORD dwSize, DWORD &dwCRC32 ); 
        static DWORD    CRC32ByFile( LPCTSTR strFileName, DWORD &dwCRC32 );

private:
        static bool     GetFileSize( const HANDLE hFile, DWORD &dwSize );
        static inline void GetCRC32( const BYTE byte, DWORD &dwCRC32 );

        static DWORD    CRC32Table[256];
};

//---------------------------------------------------------------------------
#endif
