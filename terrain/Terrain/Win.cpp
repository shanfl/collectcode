/*-----------------------------------------------------------------------------

  Win.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------

  Create the main window and make it go.

-----------------------------------------------------------------------------*/

#define CONSOLE_SIZE            30000
#define CONSOLE_PAGE            1000
#define MOUSE_MOVEMENT          0.4f

#include <windows.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "app.h"
#include "camera.h"
#include "console.h"
#include "macro.h"
#include "math.h"
#include "map.h"
#include "pointer.h"
#include "win.h"

static HWND         hwnd;
static HINSTANCE    module;
static int          width;
static int          height;
static bool         lmb;
static bool         rmb;
static bool         mouse_forced;
static point        mouse_pos;
static point        select_pos;

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CenterCursor ()
{

  int             center_x;
  int             center_y;
  RECT            rect;

  SetCursor (NULL);
  mouse_forced = true;
  GetWindowRect (hwnd, &rect);
  center_x = rect.left + (rect.right - rect.left) / 2;
  center_y = rect.top + (rect.bottom - rect.top) / 2;
  SetCursorPos (center_x, center_y);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void MoveCursor (int x, int y)
{

  int             center_x;
  int             center_y;
  RECT            rect;

  SetCursor (NULL);
  mouse_forced = true;
  GetWindowRect (hwnd, &rect);
  center_x = rect.left + x;
  center_y = rect.top + y;
  SetCursorPos (center_x, center_y);

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


  RECT            r;
  float           delta_x, delta_y;
  point           p;

	switch (message) 	{
  case WM_MOVE:
    GetClientRect (hwnd, &r);
    height = r.bottom - r.top;
    width = r.right - r.left;
    break;
  case WM_LBUTTONDOWN:
    lmb = true;
    SetCapture (hwnd);
    break;
  case WM_RBUTTONDOWN:
    rmb = true;
    SetCapture (hwnd);
    break;
  case WM_LBUTTONUP:
    lmb = false;
    if (!rmb) {
      ReleaseCapture ();
      MoveCursor (select_pos.x, select_pos.y);
    }
    break;
  case WM_RBUTTONUP:
    rmb = false;
    if (!lmb) {
      ReleaseCapture ();
      MoveCursor (select_pos.x, select_pos.y);
    }
    break;
  case WM_MOUSEMOVE:
    p.x = LOWORD(lParam);  // horizontal position of cursor 
    p.y = HIWORD(lParam);  // vertical position of cursor 
    if (!mouse_forced && !lmb && !rmb) {
      select_pos = p; 
    }
    if (mouse_forced) {
      mouse_forced = false;
    } else if (rmb || lmb) {
      CenterCursor ();
      delta_x = (float)(mouse_pos.x - p.x) / MOUSE_MOVEMENT;
      delta_y = (float)(mouse_pos.y - p.y) / MOUSE_MOVEMENT;
      if (rmb && lmb) {
        CameraSelectionYaw (delta_x);
        CameraSelectionZoom (-delta_y);
      } else if (rmb) {
        CameraPan (delta_x);
        CameraForward (delta_y);
      } else if (lmb) {
        CameraSelectionYaw (delta_x);
        CameraSelectionPitch (delta_y);
      }
    }
    mouse_pos = p;
    break;
  case WM_CLOSE:
    AppQuit ();
    return 0;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);   

}

/*-----------------------------------------------------------------------------
                                    n o t e
-----------------------------------------------------------------------------*/

void WinPopup (char* message, ...)
{

  va_list  		marker;
  char        buf[1024];

  va_start (marker, message);
  vsprintf (buf, message, marker); 
  va_end (marker);
  MessageBox (NULL, buf, APP_TITLE, MB_ICONSTOP | MB_OK | 
    MB_TASKMODAL);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int WinWidth (void)
{

  return width;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void WinMousePosition (int* x, int* y)
{

  *x = select_pos.x;
  *y = select_pos.y;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int WinHeight (void)
{

  return height;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void WinTerm (void)
{

  DestroyWindow (hwnd);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

HWND WinHwnd (void)
{

  return hwnd;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

bool WinInit (void)
{

  WNDCLASSEX wcex;

	wcex.cbSize         = sizeof(WNDCLASSEX); 
	wcex.style			    = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	  = (WNDPROC)WndProc;
	wcex.cbClsExtra		  = 0;
	wcex.cbWndExtra		  = 0;
	wcex.hInstance		  = AppInstance ();
	wcex.hIcon			    = NULL;
	wcex.hCursor		    = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
	wcex.lpszMenuName	  = NULL;
	wcex.lpszClassName	= APP_TITLE;
	wcex.hIconSm		    = NULL;

  if (!RegisterClassEx(&wcex)) {
    WinPopup ("Cannot create window class");
    return false;
  }
  if (!(hwnd = CreateWindowEx (0, APP_TITLE, APP_TITLE, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    CW_USEDEFAULT, 0, 544, 640, NULL, NULL, AppInstance (), NULL))) {
    WinPopup ("Cannot create window");
    return false;
  }
  ShowWindow (hwnd, SW_SHOW);
  UpdateWindow (hwnd);
  return true;

}