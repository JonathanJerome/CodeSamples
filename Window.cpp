/*Start Header*****************************************************************/
/*!
\file	Window.cpp
\author <Jonathan Jerome, jonathan.jerome, and 390001516>
\par	<jonathan.jerome@digipen.edu>
\date	<Oct 09, 2018>
\brief	Class that wraps around WINAPI to draw native window on Microsoft Windows 10

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header*******************************************************************/

// usr
#include "Window.h"
#include "ImGuiEditor.h"
#include "GHeaders.h"
#include "CoreEngine.h"


#include <WinUser.h>
#include <windowsx.h>
#include <assert.h>
#include "resource.h"

#include <chrono>


bool m_hasfocus = true;

LRESULT WINAPI MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{						
	
	if(g_WindowActive == true)
		f_ImGuiInput(msg, wParam, lParam);
	
	switch (msg) {
	case WM_DESTROY:	
		PostQuitMessage(0);		//A destroy message--time to kill the game. Make sure we shut everything down properly by telling Windows to post a WM_QUIT message (the parameter is the exit code)
        break;
    case WM_LBUTTONDBLCLK:
        g_Input->m_lmdc = true;
        break;
	case WM_LBUTTONDOWN:
		if(engine->m_EditorActive == false)
			getsys(Window)->SetCursorClicked();
		break;
    case WM_RBUTTONDBLCLK:
        g_Input->m_rmdc = true;
        break;

	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT)
		{
			if (engine->m_EditorActive)
			{
				SetCursor(getsys(Window)->cursor);
			}
			else
			{
				SetCursor(getsys(Window)->m_normal);
			}
			
			return TRUE;
		}
		break;

	case WM_KILLFOCUS:
		m_hasfocus = false;
		if(g_WindowActive == true)
		getsys(AudioManager)->PauseAllAudio();
		break;

	case WM_SETFOCUS:
		m_hasfocus = true;
		if (g_WindowActive == true)
		getsys(AudioManager)->ResumeAllAudio();
		break;

	case WM_MOUSEWHEEL:
		g_Input->f_Scrolling((short)HIWORD(wParam));
		break;

    
    case WM_MOUSEMOVE:
		g_MousePosition = MAKEPOINTS(lParam);
        break;
    default:
      break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Window()
	:cursor{NULL}
{
	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	SCREEN_WIDTH = ScreenWidth;
	SCREEN_HEIGHT = ScreenHeight;

	//HICON Icon = static_cast<HICON>(::LoadImage(hInstance, MAKEINTRESOURCE("Shaman_Icon.ico"), IMAGE_ICON, 48, 48,LR_DEFAULTCOLOR));

	WNDCLASSEX wc = {};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC | CS_DBLCLKS;
	wc.hIconSm = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MyIcon));;
	wc.lpszMenuName = 0;
	wc.style = 0;
	wc.hbrBackground = 0;
	wc.lpfnWndProc = (WNDPROC)MessageHandler;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = "Window Register";

	RegisterClassEx(&wc);

	hInstance = wc.hInstance;

	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&wr, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), FALSE);
	hWnd = CreateWindowEx(0,
		"Window Register",
		(LPCSTR)"SHAMAN",
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
		CW_USEDEFAULT,									//The x and y position of the window -screen coords for base window
		CW_USEDEFAULT,									//The x and y position of the window -relative coords for child windows
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		GetDesktopWindow(),								//The parent window
		NULL,
		wc.hInstance,
		NULL);

	assert(hWnd != NULL);

	LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU) & CS_DBLCLKS;
	SetWindowLong(hWnd, GWL_STYLE, lStyle);

	
	cursor = LoadCursor(NULL, IDC_ARROW);
	m_normal = LoadCursorFromFile("Cursor\\Cursor_OFF.cur");
	m_hovered = LoadCursorFromFile("Cursor\\Cursor_ON.cur");
	m_clicked = LoadCursorFromFile("Cursor\\Cursor_ON.cur");

	DragAcceptFiles(hWnd, true);

	f_HideWindow();
}

Window::~Window()
{
	UnregisterClass("Window Register", hInstance);
}

int Window::f_initialize()
{
	g_WindowActive = true;
	
	memset(&lpDevMode, 0, sizeof(DEVMODE));
	lpDevMode.dmSize = sizeof(DEVMODE);
	lpDevMode.dmDriverExtra = 0;

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &lpDevMode);
		//framerate_limit = 60; // default value if cannot retrieve from user settings.
    refreshHz = lpDevMode.dmDisplayFrequency;
	if (refreshHz < 60)
		refreshHz = 60;
	
	
	return 0;
	
}

int Window::f_reinitialize()
{
	return 0;
}

int Window::f_load()
{
	return 0;
}

int Window::f_save()
{
	return 0;
}

int Window::f_imgui()
{
  return 0;
}

void Window::f_startup()
{

}

void Window::f_update()
{
	std::chrono::steady_clock::time_point lasttime, currenttime;
	lasttime = std::chrono::steady_clock::now();

	MSG msg;
	
	//refreshHz = lpDevMode.dmDisplayFrequency;

	UpdateWindow(hWnd);
	//PeekMessage
	//Look for any pending windows messages, remove them, then handle them
	//The second parameter is the window handle--NULL just means get any message from the current thread
	//The third and forth parameters are the start and end message types to process
	//The last parameter determines whether or not the message is removed
	//It is important to get all windows messages available not just one
	while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&msg);	//Makes sure WM_CHAR and similar messages are generated
		DispatchMessage(&msg);	//Calls the message procedure (see below) with this message

		//If we get a quit message, broadcast it to all systems
		if (msg.message == WM_QUIT)
		{
			engine->m_EngineActive = false;
		
		}


	}

	if (m_hasfocus == true && engine->m_EditorActive == false)
	{
		f_wrapcursor();
	}
	currenttime = std::chrono::steady_clock::now();
	g_Timer->m_Windows = static_cast<std::chrono::duration<float>>(currenttime - lasttime).count();
}

void Window::f_reboot()
{
}

void Window::f_shutdown()
{
}

HWND Window::GetHandle()
{
  return hWnd;
}

void Window::f_wrapcursor()
{
	POINT pos;
	GetCursorPos(&pos);
	if (pos.x == SCREEN_WIDTH - 1)
		SetCursorPos(0, pos.y);
	if (pos.x == 0)
		SetCursorPos(SCREEN_HEIGHT - 1, pos.y);
	/*if (pos.y == SCREEN_HEIGHT - 1)
		SetCursorPos(pos.x, 0);
	if (pos.y == 0)
		SetCursorPos(pos.x, SCREEN_HEIGHT - 1);*/
}

void Window::f_hidecur()
{
	while (ShowCursor(false) >= 0);
}

void Window::f_showcur()
{
	while (ShowCursor(true) <= 0);
}

void Window::f_togglefullscreen()
{
	if (m_fullscreen == false)
	{
		SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
		SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, SCREEN_WIDTH,  SCREEN_HEIGHT, SWP_SHOWWINDOW);

		DEVMODE dm;
		dm.dmSize = sizeof(DEVMODE);
		dm.dmPelsWidth = SCREEN_WIDTH;
		dm.dmPelsHeight = SCREEN_HEIGHT;
		dm.dmBitsPerPel = 24;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
		m_fullscreen = true;

	}
	else
	{
		
		RECT wr = { 0, 0, 1280, 720 };
		AdjustWindowRect(&wr, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), FALSE);
		MoveWindow(hWnd, 0, 0, wr.right - wr.left, wr.bottom - wr.top, TRUE);
		ChangeDisplaySettings(0, 0);

		LONG lStyle = GetWindowLong(hWnd, GWL_STYLE);
		lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU) & CS_DBLCLKS;
		SetWindowLong(hWnd, GWL_STYLE, lStyle);
		m_fullscreen = false;
	}
	DragAcceptFiles(hWnd, true);
}

void Window::SetCursorHovered()
{
	SetCursor(m_hovered);
}

void Window::SetCursorNormal()
{
	SetCursor(m_normal);
}

void Window::SetCursorClicked()
{
	SetCursor(m_clicked);
}

void Window::SetCursorEditor()
{
	SetCursor(cursor);
}

bool Window::f_fullscreen()
{
	return m_fullscreen;
}

void Window::f_ShowWindow()
{
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

void Window::f_HideWindow()
{
	ShowWindow(hWnd, SW_HIDE);
}
