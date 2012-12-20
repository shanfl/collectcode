/*-----------------------------------------------------------------------------

  App.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------

  This module contains the core of the program.  

-----------------------------------------------------------------------------*/

#include <windows.h>

#include "app.h"
#include "camera.h"
#include "console.h"
#include "entity.h"
#include "glTypes.h"
#include "map.h"
#include "maptexture.h"
#include "render.h"
#include "texture.h"
#include "win.h"
#include "world.h"

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "GLaux.lib")

static bool           quit;
static HINSTANCE      instance;

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

HINSTANCE AppInstance ()
{

  return instance;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void AppQuit ()
{

  quit = true;

}

/*-----------------------------------------------------------------------------
                                W i n M a i n
-----------------------------------------------------------------------------*/

int PASCAL WinMain (HINSTANCE instance_in, HINSTANCE previous_instance,
  LPSTR command_line, int show_style)
{

 	MSG		  msg;
  instance = instance_in;
  WinInit ();
  ConsoleInit ();
  CameraInit ();
  RenderInit ();
  TextureInit ();
  MapInit ();
  MapTextureInit ();
  WorldInit ();
  EntityInit ();
	while (!quit) {
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	{
			if (msg.message == WM_QUIT)	
				quit = true;
			else {
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
    } else	{
      //update
      CameraUpdate ();
      EntityUpdate ();
      MapUpdate ();
      MapTextureUpdate ();
      WorldUpdate ();
      RenderUpdate ();
      Sleep (1);
    }
  }
  EntityTerm ();
  TextureTerm ();
  MapTerm ();
  MapTextureTerm ();
  WorldTerm ();
  RenderTerm ();
  CameraTerm ();
  ConsoleTerm ();
  WinTerm ();
  return 0;

}

