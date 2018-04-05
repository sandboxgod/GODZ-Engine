/* ===========================================================
	Main header file

	Created Apr 3, '04 by Richard 'vajuras' Osborne
	Copyright (c) 2004, Godlike Development
	========================================================== 
*/

#if !defined(_GODZSKINEXP_H_)
#define _GODZSKINEXP_H_

#include <windows.h>
#include <string>
#include "Max.h"

namespace GODZ
{
	// The unique ClassID
	#define GODZSKINEXP_CLASS_ID    Class_ID(0x6cf24774, 0x6a0b2521)

	// Class ID of the skin modifier included in 3DStudio R3
	// This modifier is part of R3 (installed), and the source is in the Max SDK under
	// \samples\modifier\bonesdef
	#define SKINMOD_CLASS_ID    Class_ID(0x0095C723, 0x00015666)

	
	bool ExportSelNode(const TCHAR *szFilename, int framesPerSec, Interface *pInterface);


	//Implements the Scene Export plugin
	class GODZSkinExp : public SceneExport 
	{
		public:

		//Constructor/Destructor
        GODZSkinExp();
        virtual ~GODZSkinExp();

        virtual int              ExtCount();                    // Number of extensions supported
        virtual const TCHAR *    Ext(int n);                    // Extension #n (i.e. "3DS")
        virtual const TCHAR *    LongDesc();                    // Long ASCII description (i.e. "Autodesk 3D Studio File")
        virtual const TCHAR *    ShortDesc();                // Short ASCII description (i.e. "3D Studio")
        virtual const TCHAR *    AuthorName();                // ASCII Author name
        virtual const TCHAR *    CopyrightMessage();            // ASCII Copyright message
        virtual const TCHAR *    OtherMessage1();            // Other message #1
        virtual const TCHAR *    OtherMessage2();            // Other message #2
        virtual unsigned int    Version();                    // Version number * 100 (i.e. v3.01 = 301)
        virtual void            ShowAbout(HWND hWnd);        // Show DLL's "About..." box
        virtual int             DoExport(   const TCHAR *name,
                                    ExpInterface *ei,
                                    Interface *i, 
                                    BOOL suppressPrompts, 
                                    DWORD options); 
        virtual BOOL SupportsOptions(int ext, DWORD options);
	};

	// This is the Class Descriptor for the XSkinExp plug-in
	class GodzSkinExpClassDesc : public ClassDesc 
	{
	public:
		int             IsPublic() {return 1;}
		void *             Create(BOOL loading = FALSE) {return new GODZSkinExp();}
		const TCHAR *    ClassName() {return _T("GODZSkelAnimExport");}
		SClass_ID        SuperClassID() {return SCENE_EXPORT_CLASS_ID;}
		Class_ID        ClassID() {return GODZSKINEXP_CLASS_ID;}
		const TCHAR*     Category() {return _T("GODZSkelAnimExport");}
		void            ResetClassParams (BOOL fileReset) {};
	};

	//Returns the Class Description object for this plugin
	ClassDesc* GetGODZSkinExpDesc();

	//Outputs debug to the log file....
	class GODZLog
	{
	public:
		static void Open(const char *filename);
		static void Log(const char* msg, ...);
		static void Close();

	private:
		static FILE *stream;
	};
}

#endif