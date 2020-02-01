/*Start Header*****************************************************************/
/*!
\file	Window.h
\author <Jonathan Jerome, jonathan.jerome, and 390001516>
\par	<jonathan.jerome@digipen.edu>
\date	<Oct 09, 2018>
\brief	Class that wraps around WINAPI to draw native window on Microsoft Windows 10

Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header*******************************************************************/


#pragma once

#include <Windows.h>
#include "System.h"

static bool g_WindowActive = false;
static POINTS g_MousePosition;
extern bool m_hasfocus;

class Window : public SubSystem
{
public:
  Window();
 ~Window() override;

 void f_startup()							override;
 void f_update()							override;
 void f_reboot()							override;
 void f_shutdown()							override;
											
 int f_initialize()							override;
 int f_reinitialize()						override;
 int f_load()								override;
 int f_save()								override;
 int f_imgui()								override;


  HWND GetHandle();
  
  void f_wrapcursor();

  void f_hidecur();
  void f_showcur();
  void f_togglefullscreen();

  HCURSOR m_normal;
  HCURSOR m_hovered;
  HCURSOR m_clicked;
  HCURSOR cursor;

  void SetCursorHovered();
  void SetCursorNormal();
  void SetCursorClicked();

  void SetCursorEditor();

  bool f_fullscreen();

  void f_ShowWindow();
  void f_HideWindow();

  unsigned int refreshHz;
  DEVMODE lpDevMode;

private:

	bool m_fullscreen = false;
	HWND hWnd;
	HINSTANCE hInstance;
	
};
