/*-----------------------------------------------------------------------------

  Render.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------
  
  This is the core of the gl rendering functions.  This contains the main 
  rendering function RenderScene (), which initiates the various 
  other renders in the other modules.

-----------------------------------------------------------------------------*/

#define RENDER_DISTANCE     2048

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>

#include "gltypes.h"
#include "camera.h"
#include "console.h"
#include "entity.h"
#include "macro.h"
#include "map.h"
#include "math.h"
#include "render.h"
#include "texture.h"
#include "win.h"
#include "world.h"

static	PIXELFORMATDESCRIPTOR pfd =			
{
	sizeof(PIXELFORMATDESCRIPTOR),			
	1,											  // Version Number
	PFD_DRAW_TO_WINDOW |			// Format Must Support Window
	PFD_SUPPORT_OPENGL |			// Format Must Support OpenGL
	PFD_DOUBLEBUFFER,					// Must Support Double Buffering
	PFD_TYPE_RGBA,						// Request An RGBA Format
	32,										    // Select Our glRgbaDepth
	0, 0, 0, 0, 0, 0,					// glRgbaBits Ignored
	0,											  // No Alpha Buffer
	0,											  // Shift Bit Ignored
	0,											  // Accumulation Buffers
	0, 0, 0, 0,								// Accumulation Bits Ignored
	16,											  // Z-Buffer (Depth Buffer)  bits
	0,											  // Stencil Buffers
	1,											  // Auxiliary Buffers
	PFD_MAIN_PLANE,						// Main Drawing Layer
	0,											  // Reserved
	0, 0, 0										// Layer Masks Ignored
};

static HDC			        hDC;
static HGLRC		        hRC;
static int              render_width;
static int              render_height;
static float            render_aspect;
static unsigned char*   buffer;

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderResize (void)		
{

  int     left, top;

  if (buffer)
    delete[] buffer;
  render_width = WinWidth ();
  render_height = WinHeight ();
  left = 0;
  top = 0;
  render_aspect = (float)render_width / (float)render_height;
  glViewport (left, top, render_width, render_height);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
	gluPerspective (45.0f, render_aspect, 0.1f, RENDER_DISTANCE);
	glMatrixMode (GL_MODELVIEW);
  buffer = new unsigned char[WinWidth () * WinHeight () * 4];


}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderTerm (void)
{

  if (!hRC)
    return;
  wglDeleteContext (hRC);
  hRC = NULL;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderInit (void)
{

  HWND                hWnd;
	unsigned		        PixelFormat;

  hWnd = WinHwnd ();
  if (!(hDC = GetDC (hWnd))) { // Did We Get A Device Context?
		WinPopup ("Can't Create A GL Device Context.");
		return;	
	}
	if (!(PixelFormat = ChoosePixelFormat(hDC,&pfd)))	{ // Did Windows Find A Matching Pixel Format?
		WinPopup ("Can't Find A Suitable PixelFormat.");
		return;	
	}
  if(!SetPixelFormat(hDC,PixelFormat,&pfd))	{  // Are We Able To Set The Pixel Format?
		WinPopup ("Can't Set The PixelFormat.");
		return;	
	}  
	if (!(hRC = wglCreateContext (hDC)))	{  // Are We Able To Get A Rendering Context?
		WinPopup ("Can't Create A GL Rendering Context.");
		return;		
	}
  if(!wglMakeCurrent(hDC,hRC))	{  // Try To Activate The Rendering Context
		WinPopup ("Can't Activate The GL Rendering Context.");
		return;		
	}
  glViewport (0, 0, WinWidth (), WinHeight ());
  glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SwapBuffers (hDC);
  RenderResize ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void RenderUpdate (void)		
{

  GLquat          light_vector;
  GLrgba          light_color;
  GLrgba          ambient_color;
  GLrgba          fog_color;
  GLvector        pos;
  GLvector        angle;

  light_vector = WorldLightQuat ();
  light_color = WorldLightColor ();
  fog_color = WorldFogColor ();
  ambient_color = WorldAmbientColor ();;  
  glViewport (0, 0, WinWidth (), WinHeight ());
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glShadeModel(GL_SMOOTH);
  glCullFace (GL_BACK);
	glDepthFunc(GL_LEQUAL);
  //glEnable (GL_FOG);  //enable this for fog
  glFogi (GL_FOG_MODE, GL_LINEAR);
  glFogf (GL_FOG_START, 484.0f);
  glFogf (GL_FOG_END, 5880.0f);
  glFogfv (GL_FOG_COLOR, &light_color.red);
  glEnable (GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
  glEnable (GL_CULL_FACE);
  glCullFace (GL_BACK);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glLoadIdentity();
  pos = CameraPosition ();
  angle = CameraAngle ();
  glRotatef (angle.x, 1.0f, 0.0f, 0.0f);
  glRotatef (angle.y, 0.0f, 1.0f, 0.0f);
  glRotatef (angle.z, 0.0f, 0.0f, 1.0f);
  glTranslatef (-pos.x, -pos.y, -pos.z);
  //This was part of a failed experiment.  I made a system to allow stuff to 
  //fade in over time, like in Grand Theft Auto.  This ius very effective,
  //but since the scene is drawn twice it is quite hard on framerate.
  if (0) {
    glDrawBuffer (GL_AUX0);
    glFogfv (GL_FOG_COLOR, &fog_color.red);
    glClearColor (0.0f, 0.0f, 1.0f, 1.0f);
    glClear (GL_DEPTH_BUFFER_BIT);
    EntityRenderFadeIn ();
 
    glDrawBuffer (GL_BACK);
    glClearColor (1.0f, 0.0f, 0.0f, 1.0f);
    glClear (GL_DEPTH_BUFFER_BIT);
    EntityRender ();

    glEnable (GL_BLEND);
    glDisable (GL_TEXTURE_2D);
    glReadBuffer (GL_AUX0);
    glDrawBuffer (GL_AUX0);
    glReadPixels (0, 0, WinWidth (), WinHeight (), GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    glReadBuffer (GL_BACK);
    glDrawBuffer (GL_BACK);
    glColor4f (1.0f, 1.0f, 1.0f, 0.1f);
    glPixelTransferf (GL_ALPHA_SCALE, WorldFade ());
    glDisable (GL_FOG);
    glDrawPixels (WinWidth (), WinHeight (), GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    glReadBuffer (GL_BACK);
    glDrawBuffer (GL_BACK);
    glPixelTransferf (GL_ALPHA_SCALE, 1.0f);
  } else { //this will just render everything once, with no fancy fade
    glClearColor (1.0f, 0.0f, 0.0f, 1.0f);
    glClear (GL_DEPTH_BUFFER_BIT);
    EntityRender ();
  }
  SwapBuffers (hDC);

}
