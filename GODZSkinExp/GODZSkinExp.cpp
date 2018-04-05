
#include "GODZSkinExp.h"
#include "CS/PhyExp.h"

using namespace GODZ;

//--- Globals ----------------------------------------------------------
static GodzSkinExpClassDesc XSkinExpDesc;
ClassDesc* GODZ::GetGODZSkinExpDesc() {return &XSkinExpDesc;}

// used by 3DS progress bar
DWORD WINAPI dummyFn(LPVOID arg)
{
    return(0);
}

//--- GODZSkinExp -------------------------------------------------------
GODZSkinExp::GODZSkinExp()
{
}

GODZSkinExp::~GODZSkinExp() 
{

}

int GODZSkinExp::ExtCount()
{
    //TODO: Returns the number of file name extensions supported by the plug-in.
    return 1;
}

const TCHAR *GODZSkinExp::Ext(int n)
{        
    //TODO: Return the 'i-th' file name extension (i.e. "3DS").
    return _T("gsa");
}

const TCHAR *GODZSkinExp::LongDesc()
{
    //TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
    return _T("GODZ Skeletal Animation File");
}
    
const TCHAR *GODZSkinExp::ShortDesc() 
{            
    //TODO: Return short ASCII description (i.e. "Targa")
    return _T("GODZ SkelAnim File");
}

const TCHAR *GODZSkinExp::AuthorName()
{            
    //TODO: Return ASCII Author name
    return _T("Richard 'vajuras' Osborne");
}

const TCHAR *GODZSkinExp::CopyrightMessage() 
{    
    // Return ASCII Copyright message
    return _T("Copyright 2004, Godlike Development");
}

const TCHAR *GODZSkinExp::OtherMessage1() 
{        
    //TODO: Return Other message #1 if any
    return _T("");
}

const TCHAR *GODZSkinExp::OtherMessage2() 
{        
    //TODO: Return other message #2 in any
    return _T("");
}

unsigned int GODZSkinExp::Version()
{                
    //TODO: Return Version number * 100 (i.e. v3.01 = 301)
    return 100;
}

void GODZSkinExp::ShowAbout(HWND hWnd)
{            
    // Optional
}

BOOL GODZSkinExp::SupportsOptions(int ext, DWORD options)
{
    return ( options == SCENE_EXPORT_SELECTED );
}

int GODZSkinExp::DoExport(const TCHAR *szFilename, ExpInterface *pExportInterface,
                        Interface *pInterface, BOOL suppressPrompts, DWORD options) 
{

    BOOL bSaveSelection = (options & SCENE_EXPORT_SELECTED);

    // return failure if no filename provided
    if (szFilename == NULL)
        return 0;

	TCHAR buf[1024]; //path to log file
	_tcscpy(buf, szFilename);
	size_t len = _tcslen(szFilename);
	//buf[len-3] = '\0';
	TCHAR *ptr = &buf[len-3];
	_tcscpy(ptr, "log");
	GODZLog::Open(buf);

    // Extract scene information
    pInterface->ProgressStart(_T("Exporting mesh data"),TRUE,dummyFn,NULL);
    pInterface->ProgressUpdate(0);

	bool bRet=ExportSelNode(szFilename, 30, pInterface);

	GODZLog::Close();
	pInterface->ProgressEnd();
    return bRet;
}
    
