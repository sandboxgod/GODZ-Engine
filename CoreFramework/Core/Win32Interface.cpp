
#include "Win32Interface.h"
#include "SceneManager.h"
#include "IDriver.h"		//render device
#include "InputSystem.h"	//engine input system
#include <CoreFramework/resource.h>		//resources stored within this DLL
#include "Timer.h"			//Timer utilities
#include "ViewportInterface.h"
#include "WEntity.h"
#include "LevelNode.h"
#include "WCamera.h"
#include "StringTokenizer.h"
#include <zmouse.h>
#include "GodzAtomics.h"
#include "GenericObjData.h"
#include "ScreenDimension.h"
#include "SceneData.h"
#include "GodzMutex.h"
#include <CoreFramework/Reflection/GenericClass.h>
#include "debug.h"


using namespace GODZ;

#pragma comment(lib, "comctl32.lib") //required so we can use Win32 controls

//Window32 Instance
Win32Interface* WI = NULL;

namespace GODZ
{
	extern HINSTANCE g_EngineHandle;

	const int WM_ADD_ACTOR=500; //popup menu item ID#
	const int WM_EDIT_ACTOR_PROPERTIES=501; //popup menu item ID#
	const int WM_REMOVE_ACTOR=502; //popup menu item ID#

	#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
	#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
	#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
}

Win32Interface::Win32Interface()
: m_width(0)
, m_height(0)
, mHWnd(0)
, m_bHideMouse(false)
, bWindowVisible(false)
, m_bActive(true)
, m_bHasMouseWheel(0)
, m_mouseX(0)
, m_mouseY(0)
, mIsRightMouseButtonDown(false)
, mIsWindowed(false)
{
	WI=this;
}

Win32Interface::~Win32Interface()
{
	if (!WindowTitle.isEmpty())
	{
		UnregisterClass( WindowTitle.c_str(), g_EngineHandle );
	}
}

void Win32Interface::CenterCursor()
{
	CenterCursor(mHWnd,m_width,m_height);
}

// Centers the mouse in the window
void Win32Interface::CenterCursor(HWND hwnd, int width, int height)
{
	// This is the center of the client area of our window
	POINT pt = { width / 2, height / 2 };
	
	// Get these coordinates in screen coordinates (the coordinate system the entire monitor uses)
	ClientToScreen(hwnd, &pt);
	SetCursorPos(pt.x, pt.y); // Set our cursor to the middle of our window
}

void Win32Interface::AddPopupMenuItem(HMENU menu, const char *text, bool bEnabled)
{
	MENUITEMINFO mii;
	ZeroMemory(&mii, sizeof(MENUITEMINFO));

	int count=GetMenuItemCount(menu);
	mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_STRING | MIIM_ID | MIIM_STATE; 

	if (bEnabled)
		mii.fState = MFS_ENABLED;
	else
	{
		mii.fState = MFS_DISABLED;
	}

    mii.hSubMenu = menu;

	const int maxstr = 1024;
	char popupText[maxstr];
	StringCopy(popupText, text, maxstr);
    mii.dwTypeData = popupText;

	mii.cch=1;
	mii.wID=500 + count;
    InsertMenuItem(menu, count, TRUE, &mii); 
}



void Win32Interface::Pause( bool bPause )
{
    static DWORD dwAppPausedCount = 0;

    dwAppPausedCount += ( bPause ? +1 : -1 );

    m_bActive         = ( dwAppPausedCount ? false : true );

    // Handle the first pause request (of many, nestable pause requests)
    if( bPause && ( 1 == dwAppPausedCount ) )
    {
        // Stop the scene from animating
        //if( m_bFrameMoving )
            
		
		//PerformanceTimer(SCENEMANAGER, TIMER_STOP );
    }

    if( 0 == dwAppPausedCount )
    {
        // Restart the timers
        //if( m_bFrameMoving )
		
		
		//PerformanceTimer(SCENEMANAGER, TIMER_START );
    }
}

void Win32Interface::SetActive(bool bActive)
{
	this->m_bActive=bActive;
}

void Win32Interface::SetupScene()
{
}

void Win32Interface::GetMousePos(int &x, int &y)
{
	x=m_mouseX;
	y=m_mouseY;
}

void Win32Interface::SetMousePos(int x, int y)
{
	m_mouseX = x;
	m_mouseY = y;
}


void Win32Interface::Release()
{
	WI = NULL;
}

LRESULT CALLBACK Win32Interface::WindowProc( HWND   hWnd, 
							 UINT   msg, 
							 WPARAM wParam, 
							 LPARAM lParam )
{


	if (WI == 0)
		return 0;

	static InputSystem *input = InputSystem::GetInstance();
	input->ResetStates();

    switch( msg )
	{
		case WM_MOUSEWHEEL:
		{
			/*
			The high-order word indicates the distance the wheel is rotated, expressed in multiples 
			or divisions of WHEEL_DELTA, which is 120. A positive value indicates that the wheel 
			was rotated forward, away from the user; a negative value indicates that the wheel was 
			rotated backward, toward the user. 
			*/
			
			short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			InputEvent mouseEvent(zDelta);
			input->PublishInputEvent(mouseEvent);
		}
		break;
		case WM_MOUSEMOVE:
		{
			WI->SetMousePos(LOWORD (lParam), HIWORD (lParam));
		}
		break;
	case WM_PAINT:
		{

		}
		break;
       case WM_ENTERMENULOOP:
            // Pause the app when menus are displayed
            WI->Pause(true);
            break;
        case WM_EXITMENULOOP:
            WI->Pause(false);
            break;
        case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;
				default:
					input->SetKeyState(wParam, KS_KeyDown);
					InputEvent keyEvent(wParam, KS_KeyDown);
					input->PublishInputEvent(keyEvent);
					break;
			}
		}
        break;

        case WM_KEYUP:
		{
			input->SetKeyState(wParam, KS_KeyJustReleased);
			InputEvent keyEvent(wParam, KS_KeyJustReleased);
			input->PublishInputEvent(keyEvent);
		}
        break;

		case WM_MBUTTONDOWN:
		{
			//LOWORD (lParam), HIWORD(lParam) --- x,y I presume?
			input->SetKeyState(wParam, KS_KeyDown);
			InputEvent mouseEvent(IC_MiddleMClick, VKeyState(wParam, KS_KeyDown), LOWORD (lParam), HIWORD(lParam));
			input->PublishInputEvent(mouseEvent);
		}
		break;

		case WM_MBUTTONUP:
		{
			//LOWORD (lParam), HIWORD(lParam) --- x,y I presume?
			input->SetKeyState(wParam, KS_KeyUp);
			InputEvent mouseEvent(IC_MiddleMClick, VKeyState(wParam, KS_KeyUp), LOWORD (lParam), HIWORD(lParam));
			input->PublishInputEvent(mouseEvent);
		}
		break;

		case WM_LBUTTONUP:
		{
			//LOWORD (lParam), HIWORD(lParam) --- x,y I presume?
			input->SetKeyState(wParam, KS_KeyUp);
			InputEvent mouseEvent(IC_LeftMClick, VKeyState(wParam, KS_KeyUp), LOWORD (lParam), HIWORD(lParam));
			input->PublishInputEvent(mouseEvent);
		}
		break;

		case WM_LBUTTONDOWN:
		{
			//LOWORD (lParam), HIWORD(lParam) --- x,y I presume?
			input->SetKeyState(wParam, KS_KeyDown);
			InputEvent mouseEvent(IC_LeftMClick, VKeyState(wParam, KS_KeyDown), LOWORD (lParam), HIWORD(lParam));
			input->PublishInputEvent(mouseEvent);
		}
		break;

		case WM_RBUTTONDOWN:
		{
			input->SetKeyState(wParam, KS_KeyDown);
			InputEvent mouseEvent(IC_RightMClick, VKeyState(wParam, KS_KeyDown), LOWORD (lParam), HIWORD(lParam));
			input->PublishInputEvent(mouseEvent);

			WI->mIsRightMouseButtonDown = true;
		}
		break;

		case WM_RBUTTONUP:
		{
			input->SetKeyState(wParam, KS_KeyUp);
			InputEvent mouseEvent(IC_RightMClick, VKeyState(wParam, KS_KeyUp), LOWORD (lParam), HIWORD(lParam));
			input->PublishInputEvent(mouseEvent);
		}
		break;

	case WM_COMMAND:
		{
            HMENU   hMenu = GetMenu(hWnd);
			int wmId, wmEvent;
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
			case ID_IMPORT_ASE_LEVEL:
				{
					godzassert(0); //dropped suppot for ASE
				}
				break;
			case ID_IMPORT_T3D:
				{
					/*
					//Imports file into current level.
					rstring filename = FileLoad(NULL,"Unreal Tournament (*.T3D)\0*.t3d","T3D");
					if (strlen(filename)>0)
					{
						const char* text = filename;
						StringTokenizer st(text, "\\/.");
						size_t numTokens = st.size();
						rstring packageName = st[numTokens - 2];
						packageName += "Models";

						SceneManager *smgr = SceneManager::GetInstance();

						godzassert(0); //feature not supported at this time....
					}
					*/
				}
				break;
				case ID_FILE_EXIT:
				{
					DestroyWindow(hWnd);
				}
				break;
				case ID_COLLISION_BONES:
					{
						/*
					RenderOptions *opt = TheSceneManager->GetDrawOptions();
					opt->bRenderBones=!opt->bRenderBones;
					if (opt->bRenderBones)
						CheckMenuItem(hMenu, ID_COLLISION_BONES, MF_CHECKED);
					else
						CheckMenuItem(hMenu, ID_COLLISION_BONES, MF_UNCHECKED);
						*/
					}
					break;
				case ID_COLLISION_SPATIALNODE:
				{
					/*
					RenderOptions *opt = TheSceneManager->GetDrawOptions();
					opt->bRenderNodeTree=!opt->bRenderNodeTree;
					if (opt->bRenderNodeTree)
						CheckMenuItem(hMenu, ID_COLLISION_SPATIALNODE, MF_CHECKED);
					else
						CheckMenuItem(hMenu, ID_COLLISION_SPATIALNODE, MF_UNCHECKED);
						*/
				}
				break;
				case ID_COLLISION_ENTITY:
				{
					/*
					RenderOptions *opt = TheSceneManager->GetDrawOptions();
					opt->bRenderEntityBounds=!opt->bRenderEntityBounds;
					if (opt->bRenderEntityBounds)
						CheckMenuItem(hMenu, ID_COLLISION_ENTITY, MF_CHECKED);
					else
						CheckMenuItem(hMenu, ID_COLLISION_ENTITY, MF_UNCHECKED);
						*/
				}
				break;
				case ID_COLLISION_AABBTREE:
				{
					/*
					RenderOptions *opt = TheSceneManager->GetDrawOptions();
					opt->bRenderCollisionTree=!opt->bRenderCollisionTree;
					if (opt->bRenderCollisionTree)
						CheckMenuItem(hMenu, ID_COLLISION_AABBTREE, MF_CHECKED);
					else
						CheckMenuItem(hMenu, ID_COLLISION_AABBTREE, MF_UNCHECKED);
						*/
				}
				break;
				case ID_FILL_WIREFRAME:
				{
					/*
					RenderOptions *opt = TheSceneManager->GetDrawOptions();
					opt->ChangeWireframeMode();
					if (opt->bWireframeMode)
						CheckMenuItem(hMenu, ID_FILL_WIREFRAME, MF_CHECKED);
					else
						CheckMenuItem(hMenu, ID_FILL_WIREFRAME, MF_UNCHECKED);
						*/
				}
					break;
				case ID_STATS_FPS:
				{
					/*
					RenderOptions *opt = TheSceneManager->GetDrawOptions();
					opt->bShowFPS=!opt->bShowFPS;
					if (opt->bShowFPS)
						CheckMenuItem(hMenu, ID_STATS_FPS, MF_CHECKED);
					else
						CheckMenuItem(hMenu, ID_STATS_FPS, MF_UNCHECKED);
						*/
				}
					break;
				case ID_EFFECTS_NORMALS:
					{
						/*
					RenderOptions *opt = TheSceneManager->GetDrawOptions();
					opt->bShowNormals=!opt->bShowNormals;
					if (opt->bShowNormals)
						CheckMenuItem(hMenu, ID_EFFECTS_NORMALS, MF_CHECKED);
					else
						CheckMenuItem(hMenu, ID_EFFECTS_NORMALS, MF_UNCHECKED);
						*/
					}
					break;
				case ID_SAVE:
					{
						
					}
					break;
				case ID_FILE_LOAD:
					{
					}
					break;
				case ID_NEW_MAP:
					{
						LevelNode *node = TheSceneManager->GetCurrentLevel();
						node->Clear();
						WI->SetupScene();
					}
					break;
				case ID_VIEW_CLASSBROWSER:
					break;
				case ID_VIEW_CHARACTERBROWSER:
					break;
				case ID_VIEW_MATERIALBROWSER:
					break;
				case WM_ADD_ACTOR:
					{
					}
					break;
				case ID_MESH_VIEW:
					break;
				default:
					return DefWindowProc(hWnd, msg, wParam, lParam);
			}
		}
		break;
		case WM_CREATE:
		{
			WI->CenterCursor(hWnd, WI->GetWidth(), WI->GetHeight()); // Make sure we start with the mouse centered
			return 0;
		}
		break;
		case WM_CLOSE:
		{
			//set the status for the window handle to invalid
			SceneData::SetWindowStatus(false);
			PostQuitMessage(0);
		}
		break;
		
		case WM_DESTROY:
		{
			//window is being destroyed....
		}
		break;

		case WM_SETFOCUS:
		{
		}
		break;
		case WM_KILLFOCUS:
		{
		}
		break;

        case WM_SIZE:
			WI->UpdateWindowStatus(wParam);
            break;
	case WM_SYSCOMMAND:
		// prevent screensaver or monitor powersave mode from starting
		if (wParam == SC_SCREENSAVE ||
			wParam == SC_MONITORPOWER)
			return 0;
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


int Win32Interface::GetWidth()
{
	return m_width;
}

int Win32Interface::GetHeight()
{
	return m_height;
}

SceneManager* Win32Interface::GetSceneManager()
{
	return SceneManager::GetInstance();
}

bool Win32Interface::IsMouseHidden() 
{
	return m_bHideMouse;
}

void Win32Interface::Present()
{
    ShowWindow( mHWnd, SW_SHOW );
    UpdateWindow( mHWnd );

	if (m_bHideMouse)
		ShowCursor(!m_bHideMouse); // Hide cursor

	bWindowVisible=true;
}


void Win32Interface::InitSettings()
{

}

void Win32Interface::RegisterWindow(const char* name)
{
	int width = 0;
	int height = 0;

	const char* wstr = GlobalConfig::m_pConfig->ReadNode("Application","PreferredWidth");
	const char* hstr = GlobalConfig::m_pConfig->ReadNode("Application","PreferredHeight");

	if (wstr != NULL)
	{
		width = atoi(wstr);
	}

	if (hstr != NULL)
	{
		height = atoi(hstr);
	}

	RegisterWindow(name, width, height);
}

void Win32Interface::RegisterWindow(const char* name, int width, int height)
{
	WindowTitle=name;

	WNDCLASSEXA GdzWin;

	ZeroMemory(&GdzWin,sizeof(WNDCLASSEXA));
	GdzWin.cbSize			= sizeof(WNDCLASSEXA);
	GdzWin.style			= CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
	GdzWin.lpfnWndProc		= WindowProc;
	GdzWin.hInstance		= g_EngineHandle;
	GdzWin.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	GdzWin.lpszClassName	= name;
	GdzWin.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);
	GdzWin.hCursor       = LoadCursor(NULL, IDC_ARROW);

	InitSettings();

	//-----------------hide mouse???
	m_bHideMouse = GlobalConfig::m_pConfig->ReadBoolean("Window","HideMouse");


	DWORD style = WS_OVERLAPPEDWINDOW;
	mIsWindowed = !GlobalConfig::m_pConfig->ReadBoolean("Application","FullScreen");

	if (width != 0 || height != 0)
	{
		//let width/height override
		mIsWindowed = true;
	}

	//TODO: If fullscreen mode don't define a menu
	if (mIsWindowed)
	{
		GdzWin.lpszMenuName	=	MAKEINTRESOURCE(IDR_MENU1);
		style = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|
		WS_CLIPSIBLINGS|WS_VISIBLE;

		//do not hide mouse when not fullscreen
		//m_bHideMouse = false;
	}
	else
	{
		style = WS_POPUP;
	}

	if (!m_bHideMouse)
	{
		SceneManager *smgr = SceneManager::GetInstance();
		smgr->SetAlwaysCenterMouseCursor(false);
	}

	if(!RegisterClassExA( &GdzWin ))
	{
		godzassert(!"Window not registered");
	}
	
	if (width == 0 || height == 0)
	{
		m_width = GetSystemMetrics(SM_CXSCREEN);
		m_height = GetSystemMetrics(SM_CYSCREEN);
	}
	else
	{
		m_width = width;
		m_height = height;
	}

	HINSTANCE value = g_EngineHandle;
	mHWnd = CreateWindowEx( NULL, WindowTitle, WindowTitle,
						     style, //| WS_VISIBLE,
					         0, 0, m_width, m_height, NULL, NULL, g_EngineHandle, NULL );


	
	debug_assert( mHWnd != NULL, "Cannot instantiate the GODZ Display" );

	SetWindowClass(mHWnd);

	InitCommonControls(); //initialize all win32 controls
}

void Win32Interface::SetWindowClass(HWND hwnd)
{
	SceneData::SetWindowStatus(true);
	this->mHWnd = hwnd;

	if (m_width == 0 || m_height == 0)
	{
		m_width= GetSystemMetrics(SM_CXSCREEN);
		m_height = GetSystemMetrics(SM_CYSCREEN);
	}

	m_bHasMouseWheel = GetSystemMetrics(SM_MOUSEWHEELPRESENT) > 0;

	SceneManager* smgr = SceneManager::GetInstance();
	smgr->SetDevice(hwnd,m_width,m_height); 
}

bool Win32Interface::Run()
{
	MSG uMsg;
	memset(&uMsg,0,sizeof(uMsg));

	//grab all of the messages we have waiting on the message queue
	while( PeekMessage( &uMsg, NULL, 0, 0, PM_REMOVE ) )
	{
		TranslateMessage( &uMsg );
		DispatchMessage( &uMsg );
	}

	return uMsg.message != WM_QUIT;
}


void Win32Interface::ShowMouseCursor(bool bShowCursor)
{
	m_bHideMouse=!bShowCursor;
	ShowCursor(bShowCursor);
}

void Win32Interface::UpdateWindowStatus(int wParam)
{
	if( SIZE_MINIMIZED == wParam )
    {
		m_bMinimized = true;
	}
	else if( SIZE_MAXIMIZED == wParam )
	{
		m_bMinimized=false;
	}
	else if( SIZE_RESTORED == wParam )
	{
		m_bMinimized=false;
	}
	else
	{
		// If we're neither maximized nor minimized, the window size 
		// is changing by the user dragging the window edges.  In this 
		// case, we don't reset the device yet -- we wait until the 
		// user stops dragging, and a WM_EXITSIZEMOVE message comes.
	}
}

rstring Win32Interface::FolderSelect(bool bDisplayFiles)
{
	rstring location;

    BROWSEINFO bi = { 0 };
    bi.lpszTitle = _T("Pick the Map file");
	if (bDisplayFiles)
		bi.ulFlags=BIF_BROWSEINCLUDEFILES;
    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
    if ( pidl != 0 )
    {
        // get the name of the folder
        TCHAR path[MAX_PATH];
        if ( SHGetPathFromIDList ( pidl, path ) )
        {
            //Log ( "Selected Folder: %s\n", path );
			location= path;
        }

        // free memory used
        IMalloc * imalloc = 0;
        if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
        {
            imalloc->Free ( pidl );
            imalloc->Release ( );
        }
    }

	return location;
}

rstring Win32Interface::FileSaveAs(HWND hWnd, char *fileExt, char *filter)
{
	char fileLocation[1024];
	fileLocation[0]='\0';

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = fileLocation;
	ofn.nMaxFile = sizeof(char) * 8192;
	ofn.lpstrFilter = filter;
	ofn.lpstrInitialDir = GetBaseDirectory();
	ofn.lpstrDefExt = fileExt;
	ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
	if( GetSaveFileNameA(&ofn) )
	{
		return fileLocation;
	}

	return "";
}

rstring GODZ::FileLoad(HWND hWnd, const char *fileExt, const char* defaultExt, const char* title)
{
	const int MAX_STR_SIZE = 1024;

	char fileLocation[MAX_STR_SIZE];
	fileLocation[0]='\0';

	char fileExtention[MAX_STR_SIZE];
	StringCopy(fileExtention, fileExt, MAX_STR_SIZE);

	char defaultExtention[MAX_STR_SIZE];
	StringCopy(defaultExtention, defaultExt, MAX_STR_SIZE);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = fileLocation;
	ofn.nMaxFile = sizeof(fileLocation);
	ofn.lpstrFilter = fileExtention;
	ofn.lpstrInitialDir = GetBaseDirectory();
	ofn.lpstrDefExt = defaultExtention;
	ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
	ofn.lpstrTitle = title;
	if( GetOpenFileNameA(&ofn) )
	{
		return fileLocation;
	}

	return "";
}
