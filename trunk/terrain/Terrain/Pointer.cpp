/*-----------------------------------------------------------------------------

  Pointer.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------
  
  This entity will take mouse positions and attempt to determine what part of 
  the terrain the user is pointing at.  This is used as an anchor point when 
  rotating the camera with the left mouse button.

  This stuff is very slapdash and doesn't work very well. The big flaw is that
  once it has a few squares in the selection buffer, it needs to sort by depth.
  Right now if you are close to the ground it may grab real estate on the wrong
  side of the hill.

  
-----------------------------------------------------------------------------*/

#define PT_SIZE                 8
#define PT_HALF                 (PT_SIZE / 2)

#include <windows.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>

#include "camera.h"
#include "console.h"
#include "macro.h"
#include "map.h"
#include "glTypes.h"
#include "texture.h"
#include "world.h"
#include "win.h"
#include "pointer.h"

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CPointer::Render ()
{

  int         cell_x, cell_y;
  GLvector    p;
  GLvector    pos = CameraPosition ();
  
  glPushAttrib (GL_POLYGON_BIT | GL_LIGHTING_BIT| GL_FOG_BIT);
  glDisable (GL_DEPTH_TEST);
  glEnable (GL_TEXTURE_2D);
  glDisable (GL_FOG);
  glEnable (GL_BLEND);
  glEnable (GL_ALPHA);
  glBindTexture (GL_TEXTURE_2D, m_texture);
  glDisable (GL_CULL_FACE);
  glBlendFunc (GL_ONE, GL_ONE);
  glLineWidth (3.5f);
  glColor3f (1.0f, 0.5f, 0.0f);
  cell_x = (int)(pos.x - 0.5f) + MapSize () / 2;
  cell_y = (int)(pos.z - 0.5f) + MapSize () / 2;
  cell_x = m_last_cell.x;
  cell_y = m_last_cell.y;
  glBegin (GL_QUADS);
  glTexCoord2f (0.0f, 0.0f);
  p = MapPosition (cell_x - PT_HALF, cell_y - PT_HALF);   
  p.x -= m_pulse;   p.y += 2.0f;  p.z -= m_pulse;
  glVertex3fv (&p.x);
  glTexCoord2f (0.0f, 1.0f);
  p = MapPosition (cell_x - PT_HALF, cell_y + PT_HALF);   
  p.x -= m_pulse;   p.y += 2.0f;  p.z += m_pulse;
  glVertex3fv (&p.x);
  glTexCoord2f (1.0f, 1.0f);
  p = MapPosition (cell_x + PT_HALF, cell_y + PT_HALF);   
  p.x += m_pulse;   p.y += 2.0f;  p.z += m_pulse;
  glVertex3fv (&p.x);
  glTexCoord2f (1.0f, 0.0f);
  p = MapPosition (cell_x + PT_HALF, cell_y - PT_HALF);
  p.x += m_pulse;   p.y += 2.0f;  p.z -= m_pulse;
  glVertex3fv (&p.x);
  glEnd ();
  glPopAttrib ();
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable (GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable (GL_CULL_FACE);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

static point DrawGrid (void)
{

  int         x, y;
  int         block;
  int	        hits;		
	unsigned int buffer[512];							// Set Up A Selection Buffer
  GLvector    v1, v2, v3, v4;
  point       cell;

  memset (buffer, 0, sizeof (buffer));
  // Tell OpenGL To Use Our Array For Selection
	glSelectBuffer(512, buffer);						
	// Put OpenGL In Selection Mode.
	glRenderMode(GL_SELECT);
	glInitNames();								
	glPushName(0);								
  block = 0;
  glDisable (GL_CULL_FACE);
  for (y = 0; y < MapSize (); y += PT_SIZE) {
    for (x = 0; x < MapSize (); x += PT_SIZE) {
      block = x + y * MapSize ();
      glLoadName (block);				
      v1 = MapPosition (x, y);
      v2 = MapPosition (x, y + PT_SIZE);
      v3 = MapPosition (x + PT_SIZE, y + PT_SIZE);
      v4 = MapPosition (x + PT_SIZE, y);
      glBegin (GL_QUADS);
      glVertex3fv (&v1.x);
      glVertex3fv (&v2.x);
      glVertex3fv (&v3.x);
      glVertex3fv (&v4.x);
      glEnd ();
    }
  }
	hits = glRenderMode(GL_RENDER);	
  cell.x = cell.y = -1;
  if (hits > 0) {
    block = buffer[3];
    cell.x = block % MapSize () + PT_HALF;
    cell.y = (block - cell.x) / MapSize () + PT_HALF;

  }    
  return cell;



}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CPointer::Update ()
{

  point       p;
	int	        viewport[4];	
  GLvector    pos;
  GLvector    angle;
  unsigned long    t;

  t = GetTickCount () % 3600;
  m_pulse = (float)sin (((float)t / 10.0f) * DEGREES_TO_RADIANS) * 1.0f;
  WinMousePosition (&p.x, &p.y);
  if (m_last_mouse.x == p.x && m_last_mouse.y == p.y)
    return;
  m_last_mouse = p;
	// This Sets The Array <viewport> To The Size And Location Of The Screen Relative To The Window
  glViewport (0, 0, WinWidth (), WinHeight ());
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();								
	glLoadIdentity();							
	// This Creates A Matrix That Will Zoom Up To A Small Portion Of The Screen
	gluPickMatrix((GLdouble) p.x, (GLdouble) (viewport[3]-p.y), 2.0f, 2.0f, viewport);
	// Apply The Perspective Matrix
	gluPerspective(45.0f, (float) (viewport[2]-viewport[0])/(float) (viewport[3]-viewport[1]), 0.1f, 1024.0f);
	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
  glLoadIdentity();
  pos = CameraPosition ();
  angle = CameraAngle ();
  glRotatef (angle.x, 1.0f, 0.0f, 0.0f);
  glRotatef (angle.y, 0.0f, 1.0f, 0.0f);
  glRotatef (angle.z, 0.0f, 0.0f, 1.0f);
  glTranslatef (-pos.x, -pos.y, -pos.z);
  m_last_cell = DrawGrid ();
	glMatrixMode(GL_PROJECTION);				
	glPopMatrix();								
	glMatrixMode(GL_MODELVIEW);		


}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CPointer::CPointer ()
{

  m_last_cell.x = m_last_cell.y = -1;
  m_texture = TextureFromName ("ring");
  m_entity_type = "pointer";

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

point CPointer::Selected ()
{

  return m_last_cell;

}
