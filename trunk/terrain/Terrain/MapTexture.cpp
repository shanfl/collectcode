/*-----------------------------------------------------------------------------

  MapTexture.cpp

  2006 Shamus Young


-------------------------------------------------------------------------------
  
  This generates the zone textures.  I'm still not happy with how this works, 
  but it can't be improved until you decide how you want to use it.

  Want static lighting? Then dump this incremental update stuff and just 
  make these texures duing your "loading screen".  This will speed up 
  framerate and make everything much simpler.

  Want moving shadows? Then this needs work. There are actually two groups of 
  textures.  One group is in use and SHOULD NOT BE CHANGED, or the user will 
  see the textures changing.  The other group is the ones being worked on now.  
  Every time one set is complete, it goes into use.  Once the old ones fall 
  out of use, you can start on them.  All of this is a drag on FPS and you 
  still have the problem of the shadows jumping whenever you switch to the new
  texture set.  

-----------------------------------------------------------------------------*/

//This defines the highest resolution of the zone textures.  Note that because
//of the we this thing draws the textures, this cannot be bigger than the 
//viewport. So, if you make the window 640x480, then textures can;t be 512,
//because the window isn't tall enough to act as a canvas for drawing a 512x512!
#define MAX_RESOLUTION      256
#define MAX_PIXELS          (MAX_RESOLUTION * MAX_RESOLUTION)
#define OPPOSITE(g)         (g==GRID_FRONT ? GRID_BACK : GRID_FRONT)
//How much time to spend drawing the next set of textures.  Tradeoffs abound.
#define UPDATE_TIME         10
//For debugging: Put the texture size onto the texture
#define SHOW_RESOLUTION     0
//how many times textures should repeat over a zone.  This has to be at least
//one.  Tune this depending on how tight you want the detail.
#define UV_SCALE            1

#include <windows.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>

#include "app.h"
#include "camera.h"
#include "console.h"
#include "glTypes.h"
#include "macro.h"
#include "math.h"
#include "texture.h"
#include "world.h"
#include "map.h"

enum
{
  GRID_FRONT,
  GRID_BACK,
  GRID_COUNT,
};

struct ztexture
{
  bool      ready;
  int       size;
  unsigned  texture;
};

static unsigned char*       buffer;
static ztexture             zone_texture[GRID_COUNT][ZONES];                 
static unsigned             layer_texture[LAYER_COUNT];
static unsigned             res_texture[MAX_RESOLUTION];
static unsigned             current_grid;
static unsigned             current_zone;
static unsigned             current_layer;
static unsigned             pixel_count;
static unsigned             build_time;
static int                  ref_count;
static int                  camera_zone_x;
static int                  camera_zone_y;
static int                  zone_size;


/*-----------------------------------------------------------------------------
Which zone is the camera over?
-----------------------------------------------------------------------------*/

static void GetCameraZone (void)
{

  GLvector          cam;
  int               zone_size;
  
  cam = CameraPosition ();
  zone_size = MapSize () / ZONE_GRID;
  camera_zone_x = ((int)cam.x + (MapSize () / 2)) / zone_size;
  camera_zone_y = ((int)cam.z + (MapSize () / 2)) / zone_size;

}

/*-----------------------------------------------------------------------------
Get the current texture for the requested zone
-----------------------------------------------------------------------------*/

unsigned MapTexture (int zone)
{

  int     grid;

  grid = OPPOSITE (current_grid);
  //if the very last texture is requested, then we know the terrain is about
  //to change over to a new set, and we can increment our ref count.
  //once this reaches 2, we know both terrains (the one being built and the 
  //one in use) are using our latest texture set, and it is safe to work on 
  //the old set.
  if (zone == ZONES - 1)
    ref_count++;
  if (!zone_texture[grid][zone].ready) 
    return 0;
  return zone_texture[grid][zone].texture;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void MapTextureInit (void)
{

  int       grid;
  int       zone;

  for (grid = GRID_FRONT; grid < GRID_COUNT; grid++) {
    for (zone =0; zone < ZONES; zone++) {
      zone_texture[grid][zone].size = 0;
      zone_texture[grid][zone].ready = false;
      glGenTextures (1, &zone_texture[grid][zone].texture);
    }
  }
  buffer = new unsigned char[MAX_PIXELS * 4];
  zone_size = MapSize () / ZONE_GRID;
  layer_texture[LAYER_GRASS] = TextureFromName ("grassa512");
  layer_texture[LAYER_LOWGRASS] = TextureFromName ("grassb512");
  layer_texture[LAYER_SAND] = TextureFromName ("sand512");
  layer_texture[LAYER_ROCK] = TextureFromName ("rock512");
  layer_texture[LAYER_DIRT] = TextureFromName ("dirt512");
  if (SHOW_RESOLUTION) { 
    res_texture[8] = TextureFromName ("n8");
    res_texture[16] = TextureFromName ("n16");
    res_texture[32] = TextureFromName ("n32");
    res_texture[64] = TextureFromName ("n64");
    res_texture[128] = TextureFromName ("n128");
    res_texture[256] = TextureFromName ("n256");
    res_texture[512] = TextureFromName ("n512");
  }
  GetCameraZone ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void MapTextureTerm (void)
{

  delete[] buffer;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

static void DrawLayer (int origin_x, int origin_y, int size, int layer)
{

  GLrgba          color1, color2;
  int             step;
  int             x, y, xx, yy, y2;
  float           cell_size;
  bool            drawing;
  bool            blank;

  glBindTexture (GL_TEXTURE_2D, layer_texture[layer]);
  cell_size = size / (float)zone_size;
  if (cell_size >= 1.0f)
    step = 1;
  else 
    step = (int)(1.0f / cell_size);
  if (layer == LAYER_GRASS) {
    glColor3f (1, 1, 1);
    glBegin (GL_QUAD_STRIP);
    glTexCoord2f (0.0f, 0.0f);
    glVertex2i (0, 0);
    glTexCoord2f (0.0f, UV_SCALE);
    glVertex2i (0, size);
    glTexCoord2f (UV_SCALE, 0);
    glVertex2i (size, 0);
    glTexCoord2f (UV_SCALE, UV_SCALE);
    glVertex2i (size, size);
    glEnd ();
    return;
  }
  if (layer == LAYER_LIGHTING) {
    glBindTexture (GL_TEXTURE_2D, 0);
    glBlendFunc (GL_DST_COLOR, GL_ZERO);
    for (y = -1; y < zone_size + step + 1; y += step) {
      yy = origin_y + y;
      y2 = y + 1;
      glBegin (GL_QUAD_STRIP);
      for (x = -1; x < zone_size + step + 1; x += step) {
        xx = origin_x + x;
        color1 = MapLight (xx, yy);
        color2 = MapLight (xx, yy + step);
        glColor3fv (&color1.red);
        glVertex2i ((int)((float)x * cell_size), (int)((float)y * cell_size));
        glColor3fv (&color2.red);
        glVertex2i ((int)((float)x * cell_size), (int)((float)(y2) * cell_size));
      }
      glEnd ();
    }
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return;
  }
  if (layer == LAYER_SPECIAL) {
    glBindTexture (GL_TEXTURE_2D, res_texture[size]);
    glBlendFunc (GL_SRC_COLOR, GL_ONE);
    if (current_grid != GRID_FRONT)
      glColor4f (0.6f, 0.0f, 0.6f, 0.3f);
    else
      glColor4f (0.6f, 0.6f, 0.0f, 0.3f);
    glBegin (GL_QUAD_STRIP);
    glTexCoord2f (0.0f, 0.0f);
    glVertex2i (0, 0);
    glTexCoord2f (0.0f, 1.0f);
    glVertex2i (0, size);
    glTexCoord2f (1.0f, 0.0f);
    glVertex2i (size, 0);
    glTexCoord2f (1.0f, 1.0f);
    glVertex2i (size, size);
    glEnd ();
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return;
  }
  color1 = color2 = glRgba (1.0f);
  for (y = -1; y < zone_size + step + step; y += step) {
    drawing = false;
    blank = true;
    yy = origin_y + y;
    y2 = y + 1;
    for (x = -1; x < zone_size + step + 1; x += step) {
      xx = origin_x + x;
      color1.alpha = MapLayer (xx, yy, layer);
      color2.alpha = MapLayer (xx, yy + step, layer);
      if (color1.alpha == 0.0f && color2.alpha == 0.0f) {
        if (blank) {
          if (drawing) {
            glEnd ();
            drawing = false;
          }
        }
        blank = true;
      } else if (!drawing) {
        glBegin (GL_QUAD_STRIP);
        blank = false;
        drawing = true;
      }
      if (drawing) {
        glColor4fv (&color1.red);
        glTexCoord2f (((float)x / (float)zone_size) * UV_SCALE, ((float)y / (float)zone_size) * UV_SCALE);
        glVertex2i ((int)((float)x * cell_size), (int)((float)y * cell_size));

        glColor4fv (&color2.red);
        glTexCoord2f (((float)x / (float)zone_size) * UV_SCALE, ((float)y2 / (float)zone_size) * UV_SCALE);
        glVertex2i ((int)((float)x * cell_size), (int)((float)y2 * cell_size));
      }
    }
    if (drawing)
      glEnd ();
  }

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void MapTextureUpdate (void)
{

  ztexture*       z;
  unsigned long   end;
  unsigned long   now;
  int             delta_x, delta_y, delta;
  int             zone_x, zone_y, origin_x, origin_y;

  if (ref_count < 2)
    return;
  now = GetTickCount ();
  end = now + UPDATE_TIME;
  while (GetTickCount () < end) {
    z = &zone_texture[current_grid][current_zone];
    zone_x = (current_zone % ZONE_GRID);
    zone_y = ((current_zone - zone_x) / ZONE_GRID);
    origin_x = zone_x * zone_size;
    origin_y = zone_y * zone_size;
    glBindTexture (GL_TEXTURE_2D, z->texture);
    if (current_layer == 0) {
      delta_x = ABS ((camera_zone_x - zone_x));
      delta_y = ABS ((camera_zone_y - zone_y));
      delta = MAX (delta_x, delta_y);
      if (delta < 2)
        z->size = MAX_RESOLUTION;
      else if (delta < 3)
        z->size = MAX_RESOLUTION / 2;
      else if (delta < 4)
        z->size = MAX_RESOLUTION / 4;
      else if (delta < 6)
        z->size = MAX_RESOLUTION / 8;
      else if (delta < 7)
        z->size = MAX_RESOLUTION / 16;
      else if (delta < 8)
        z->size = MAX_RESOLUTION / 32;
      else 
        z->size = MAX_RESOLUTION / 64;
      z->size = CLAMP (z->size, 8, 512);
      glTexImage2D (GL_TEXTURE_2D, 0, 3, z->size, z->size, 0, 
        GL_RGB, GL_UNSIGNED_BYTE, buffer);
    }
    glEnable (GL_TEXTURE_2D);
    glDisable (GL_CULL_FACE);
    glDisable (GL_LIGHTING);
    glDisable (GL_DEPTH_TEST);
    glDepthMask (FALSE);
    glLoadIdentity ();
    glViewport (0, 0, z->size, z->size);
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glLoadIdentity ();
    glOrtho (0.0f, z->size, 0, z->size, 0, 1.0f);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable (GL_BLEND);
    glBindTexture (GL_TEXTURE_2D, z->texture);
    glColor3f (1.0f, 1.0f, 1.0f);
    glBegin (GL_QUAD_STRIP);
    glTexCoord2f (0.0f, 0.0f);
    glVertex2i (0, 0);
    glTexCoord2f (0.0f, 1.0f);
    glVertex2i (0, z->size);
    glTexCoord2f (1.0f, 0.0f);
    glVertex2i (z->size, 0);
    glTexCoord2f (1.0f, 1.0f);
    glVertex2i (z->size, z->size);
    glEnd ();

    while (current_layer != LAYER_COUNT && GetTickCount () < end) {
      DrawLayer (origin_x, origin_y, z->size, current_layer);
      current_layer++;
    }
    if (SHOW_RESOLUTION && current_layer == LAYER_COUNT) 
      DrawLayer (origin_x, origin_y, z->size, LAYER_SPECIAL);
    if (current_layer == LAYER_COUNT) 
      DrawLayer (origin_x, origin_y, z->size, LAYER_LIGHTING);
    //save the results in our block texture
    glBindTexture (GL_TEXTURE_2D, z->texture);
    glReadPixels (0, 0, z->size, z->size, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glTexImage2D (GL_TEXTURE_2D, 0, 3, z->size, z->size, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    //enable the following line to enable mipmapping. 
    //gluBuild2DMipmaps (GL_TEXTURE_2D, 4, TEXTURE_SIZE, TEXTURE_SIZE, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    //restore the projection matrix and cleanup
    glEnable (GL_DEPTH_TEST);
    glDepthMask (TRUE);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);
    if (current_layer == LAYER_COUNT) { // did we just finish this texture?
      pixel_count += z->size * z->size;
      current_layer = 0;
      z->ready = true;
      current_zone++;
      if (current_zone == ZONES) {
        //little debug stuff here.  Figure out how many mb of mem we just ate.
        float     meg = (float)pixel_count / 1048576.0f;
        Console ("Zonemap: %1.2fmb of data in %dms", meg * 4.0f, build_time);
        pixel_count = 0;
        build_time = 0;
        current_grid = OPPOSITE (current_grid);
        current_zone = 0;
        ref_count = 0;
        GetCameraZone ();
      }
    }
  }
  build_time += GetTickCount () - now;

}
