/*-----------------------------------------------------------------------------

  Map.cpp

  2006 Shamus Young


-------------------------------------------------------------------------------
  
  This module handles the saving, loading, and creation of terrain DATA.  The 
  terrain mesh is generated in terrain.cpp, but this is where the elevation 
  data comes from.  

  This module loads in a bitmap and does some fancy stuff on it to make it look
  nice.  This takes a while, so the first time you run the app, it will take 
  some time to generate this.  Once done, it will save the map so it can skip
  the creation step next time.

  Really, in a production environment the stuff to CREATE a map would go in a 
  different program altogether.


-----------------------------------------------------------------------------*/

//This defines how big the map is. This MUST be a power of 2
#define MAP_AREA          1024 
//these are derived from the above.  Don't mess with these
#define MAP_SIZE          (MAP_AREA + 1)
#define MAP_HALF          (MAP_AREA / 2)
#define MAP_CELLS         (MAP_SIZE * MAP_SIZE)  
//this is the name of the bitmap to use when generating data
#define MAP_IMAGE         "terrain_map6.bmp"
//this is the name of the raw data file where we dump the terrain data 
//so we don't have to generate it every time we run the program.
#define MAP_FILE          "map.dat"
//We smooth out incoming elevation data.  This is the radius of the circle
//used in blending.  Larger values will make the hills smoother. A value of 0
//will disable the smoothing, making the terrain more jagged and higher poly.
#define BLEND_RANGE       3
//How many milliseconds to spend updating the lighting and shadows
//Keep in mind we don't need much
#define UPDATE_TIME       1
//Here are some macros to make the code more readble and easier to type
#define SCALE(x,y)        scale[x + y * MAP_SIZE]
#define LIMIT(x)          (CLAMP(x,0,MAP_SIZE))
#define CELL(x,y)         (LIMIT(x) + LIMIT(y) * MAP_SIZE)
//This controls the radius of the spherical area around the camera where 
//terrain data is concentrated.  That is, stuff right next to the camera is 
//the highest detail, and will diminish until it reaches FAR_VIEW, where detail 
//is lowest.
#define FAR_VIEW          200
//How high to make the hills.  You'll need to tune this if you use different 
//terrain data
#define TERRAIN_SCALE     90
//set this to 1 to make it re-generate the terrain data instead of loading the
//data file each time.
#define FORCE_REBUILD     0


#include <windows.h>
#include <math.h>
#include <stdio.h>

#include "app.h"
#include "camera.h"
#include "console.h"
#include "glTypes.h"
#include "macro.h"
#include "math.h"
#include "texture.h"
#include "world.h"
#include "map.h"

struct cell
{
  unsigned char layer[LAYER_COUNT - 1];
  bool          shadow;
  float         distance;//move this to low-res sub-map?
  GLvector      position;
  GLvector      normal;
  GLrgba        light;
};  

static struct cell    map[MAP_SIZE][MAP_SIZE];
static HDC            dc;
static HPEN           pen;
static int            bits;
static int            scan_y;

/*-----------------------------------------------------------------------------
This will take the given elevations and calculte the resulting surface normal.
-----------------------------------------------------------------------------*/

static GLvector DoNormal (float north, float south, float east, float west)
{

  GLvector    result;

  result.x = west - east;
  result.y = 2.0f;
  result.z = north - south;
  return glVectorNormalize (result);

}


/*-----------------------------------------------------------------------------
GetPixel is SLOW!  It takes a ridiculous ammount of time to sample colors using
GetPixel, so we get the raw data and use this function to extract arbitrary 
bits and use them to fill a full byte
-----------------------------------------------------------------------------*/

static unsigned char rgb_sample (short val, int shift, int numbits)
{

  unsigned char     r;

  r = val >> shift;
  r &= (int)(pow (2, numbits) - 1);
  r = r << (8 - numbits);
  return r & 255;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void MapSave ()
{

  FILE*      f;

  f = fopen (MAP_FILE, "wb");
  fwrite (map, sizeof (map), 1, f);
  fclose (f);
  Console ("Saved %s (%d bytes)", MAP_FILE, sizeof (map));

}

/*-----------------------------------------------------------------------------

  This will generate the terrain data.  

  This code is a mess. It's full of magic numbers that are tuned to work with 
  the current bitmap image, but a different terrain will probably need
  all sorts of tweaks.

  This one function is really our "level editor", and it should be a 
  seperate program with all sorts of options for controling these magic 
  numbers.  

-----------------------------------------------------------------------------*/

void MapBuild (void)
{

  HBITMAP         basemap;
  BITMAPINFO      bmi;
  GLrgba*         cmap;
  cell*           c;
  short           val;
  int             width, height;
  int             x, y;
  int             xx, yy;
  int             max_x, max_y;
  int             left;
  int             right;
  int             top;
  int             bottom;
  int             samples;
  float           high, low;
  float           smooth;
  float           e;
  unsigned char   r, g, b;
  unsigned char*  basebits;  
  float*          scale;
  
  //get a couple of temp buffers to use below
  scale = new float[MAP_CELLS];
  cmap = new GLrgba [MAP_CELLS];
  Console ("size = %d", sizeof (cell));
  //now load the bitmap
  dc = CreateCompatibleDC (GetDC (NULL));
  bits = GetDeviceCaps (dc, BITSPIXEL);
  pen = CreatePen (PS_SOLID, 1, RGB (0, 255, 0));
  basemap = (HBITMAP)LoadImage (AppInstance (), MAP_IMAGE,
    IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_VGACOLOR);
  if (!basemap) { 
    Console ("MapInit: Unable to load %s", MAP_IMAGE);
    return;
  } 
  Console ("MapBuild: rebuilding map data.");
  //call this to fill in the bmi with good values
  ZeroMemory (&bmi, sizeof (BITMAPINFO));
  bmi.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
  SelectObject (dc, basemap);
  GetDIBits (dc, basemap, 0, 0, NULL, &bmi, DIB_RGB_COLORS);
  width = bmi.bmiHeader.biWidth;
  height = bmi.bmiHeader.biHeight;
  basebits = new unsigned char[bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4];
  GetBitmapBits (basemap, bmi.bmiHeader.biSizeImage, basebits);
  max_x = MIN (bmi.bmiHeader.biWidth, MAP_SIZE);
  max_y = MIN (bmi.bmiHeader.biHeight, MAP_SIZE);
  high = -9999.0f;
  low = 9999.0f;
  //now pass over the image and convert the color values to elevation values.
  for (y = 0; y < MAP_SIZE; y++) {
    for (x = 0; x < MAP_SIZE; x++) {
      if (x == MAP_AREA && y == MAP_AREA)
        x = x;
      xx = CLAMP (x, 1, (max_x - 2));
      yy = CLAMP (y, 1, (max_y - 2));

      if (bits == 32) {
        b = basebits[(xx + yy * width) * 4];
        g = basebits[(xx + yy * width) * 4 + 1];
        r = basebits[(xx + yy * width) * 4 + 2];
      } else { //we are dealing with a 16 bit color value, use first 5 bits
        memcpy (&val, &basebits[(xx + yy * width) * 2], 2); 
        b = rgb_sample (val, 0, 5); 
        g = rgb_sample (val, 6, 5); 
        r = rgb_sample (val, 11, 5); 
      }
      //Now we have the rgb values, scale them however seems best.
      e = (float)(r) / 30.0f; 
      //(not using the blue channel right now
      e += (float)(g) / 48.0f;
      //now store the position in our grid of data
      map[x][y].position.x = (float)(x - MAP_HALF);
      map[x][y].position.y = e;
      map[x][y].position.z = (float)(y - MAP_HALF);
      map[x][y].shadow = false;
      //keep track of high/low, which is used to normalize the data later
      high = MAX (high, map[x][y].position.y);
      low = MIN (low, map[x][y].position.y);
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  //convert elevations to scalar values 0.0 - 1.0 and copy them to the temp grid
  /////////////////////////////////////////////////////////////////////////////
  for (x = 0; x < MAP_SIZE; x++) {
    for (y = 0; y < MAP_SIZE; y++) {
      c = &map[x][y];
      SCALE(x,y) = (c->position.y - low) / (high - low);
    }
  }
  /////////////////////////////////////////////////////////////////////////////
  //calculate the surface normals
  /////////////////////////////////////////////////////////////////////////////
  for (x = 0; x < MAP_SIZE; x++) {
    for (y = 0; y < MAP_SIZE; y++) {
      c = &map[x][y];
      top = WRAP ((y - 1), MAP_SIZE);
      bottom = WRAP ((y + 1), MAP_SIZE);
      left = WRAP ((x - 1), MAP_SIZE);
      right = WRAP ((x + 1), MAP_SIZE);
      c->normal = DoNormal (SCALE(x,top) * TERRAIN_SCALE, 
        SCALE(x,bottom) * TERRAIN_SCALE,
        SCALE(right,y) * TERRAIN_SCALE, 
        SCALE(left,y) * TERRAIN_SCALE);
    }
  } 
  /////////////////////////////////////////////////////////////////////////////
  //Blend the values in the temp grid and convert back to elevation values
  /////////////////////////////////////////////////////////////////////////////
  for (x = 0; x < MAP_SIZE; x++) {
    for (y = 0; y < MAP_SIZE; y++) {
      c = &map[x][y];
      smooth = 0.0f;
      samples = 0;
      for (xx = -BLEND_RANGE; xx <= BLEND_RANGE; xx++) {
        for (yy = -BLEND_RANGE; yy <= BLEND_RANGE; yy++) {
          smooth += SCALE(CLAMP ((x + xx), 0, MAP_AREA), CLAMP ((y + yy), 0, MAP_AREA));
          samples++;
        }
      }
      c->position.y = (smooth / (float)samples) * TERRAIN_SCALE;
    }
  }  

  /////////////////////////////////////////////////////////////////////////////
  //calculate the distances
  /////////////////////////////////////////////////////////////////////////////
  for (x = 0; x < MAP_SIZE; x++) {
    for (y = 0; y < MAP_SIZE; y++) {
      c = &map[x][y];
      c->distance = glVectorLength (glVectorSubtract (c->position, CameraPosition ()));
      c->distance /= FAR_VIEW;
      c->distance = CLAMP (c->distance, 0.0f, 1.0f);
    }
  }
  /////////////////////////////////////////////////////////////////////////////
  //calculate the layers.  This will look at how high and steep each point is, 
  //and then determine how much rock, sand, low grass, and dirt each point has.
  //Note that we do this using the scalar data, which HAS NOT BEEN SMOOTHED.
  //This means all those coarse bumps on the terrain are still available, and 
  //they give lots of detail and variation to this surface. This data is used 
  //to generate the zone textures. Go nuts here. The more complex the surface 
  //is, the more interesting it is to look at, and it doesn't take any more 
  //time to render.  If we did this using the smoothed out data, it would be 
  //mostly grass everywhere, which would be boring.
  /////////////////////////////////////////////////////////////////////////////
  for (x = 0; x < MAP_SIZE; x++) {
    for (y = 0; y < MAP_SIZE; y++) {
      c = &map[x][y];
      c->layer[LAYER_LOWGRASS - 1] = 0;
      c->layer[LAYER_DIRT - 1] = 0;
      c->layer[LAYER_SAND - 1] = 0;
      c->layer[LAYER_ROCK - 1] = 0;
      //sand is in the lowest parts of the map
      smooth = MathSmoothStep (SCALE(x,y), 0.3f, 0.1f);
      c->layer[LAYER_SAND - 1] = (int)(smooth * 255.0f);
      //the deep lush grass likes lowlands and flat areas
      e = MathSmoothStep (c->normal.y, 0.75f, 1.0f);
      smooth = MathSmoothStep (SCALE(x,y), 0.45f, 0.25f);
      smooth = (e * smooth) * 5.0f;
      smooth = CLAMP (smooth, 0, 1);
      c->layer[LAYER_LOWGRASS - 1] = (int)(smooth * 255.0f);
      //rock likes mild slopes and high elevations
      e = MathSmoothStep (c->normal.y, 0.8f, 0.5f);
      e += MathSmoothStep (SCALE(x,y), 0.7f, 1.0f);
      smooth = CLAMP (e, 0, 1);
      c->layer[LAYER_ROCK - 1] = (int)((smooth) * 255.0f);
      //dirt likes very steep slopes
      e = MathSmoothStep (c->normal.y, 0.7f, 0.4f);
      c->layer[LAYER_DIRT - 1] = (int)(e * 255.0f);

    
    }
  }  
  /////////////////////////////////////////////////////////////////////////////
  //Now we re-calculate the surface normals.  The values we calculated before 
  //were based on the un-smoothed terrain data, which we needed in the previous 
  //step.  Now update the normals with the (more correct) smoothed data.
  /////////////////////////////////////////////////////////////////////////////
  for (x = 0; x < MAP_SIZE; x++) {
    for (y = 0; y < MAP_SIZE; y++) {
      c = &map[x][y];
      top = WRAP ((y - 1), MAP_SIZE);
      bottom = WRAP ((y + 1), MAP_SIZE);
      left = WRAP ((x - 1), MAP_SIZE);
      right = WRAP ((x + 1), MAP_SIZE);
      c->normal = DoNormal (map[x][top].position.y, 
        map[x][bottom].position.y,
        map[right][y].position.y, 
        map[left][y].position.y);
    }
  } 
  //All done.  Let's clean up and store this thing.
  delete []cmap;
  delete []scale;
  delete []basebits;
  DeleteObject (basemap);
  MapSave ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

bool MapLoad ()
{

  FILE*       f;
  int         r;

  f = fopen (MAP_FILE, "rb");
  if (!f) {
    Console ("MapLoad: Unable to load %s", MAP_FILE);
    return false;
  }
  r = fread (map, sizeof (map), 1, f);
  if (r < 1) {
    Console ("MapLoad: Error loading %s", MAP_FILE);
    return false;
  }
  fclose (f);
  return true;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void MapInit (void)
{

  if (!MapLoad () || FORCE_REBUILD)
    MapBuild ();

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

float MapElevation (int x, int y)
{
  
  x = CLAMP (x, 0, MAP_AREA);
  y = CLAMP (y, 0, MAP_AREA);
  return map[x][y].position.y;

}


/*-----------------------------------------------------------------------------
Get the elevation of an arbitrary point over the terrain.  This will 
interpolate between points so that we can have collision with the suface.
-----------------------------------------------------------------------------*/

float MapElevation (float x, float y)
{

  int     cell_x;
  int     cell_y;
  float   a;
  float   b;
  float   c;
  float   y0, y1, y2, y3;
  float   dx;
  float   dy;

  cell_x = (int)x;
  cell_y = (int)y;
  dx = (x - (float)cell_x);
  dy = (y - (float)cell_y);
  cell_x += MAP_HALF;
  cell_y += MAP_HALF;
  y0 = MapElevation (cell_x, cell_y);
  y1 = MapElevation (cell_x + 1, cell_y);
  y2 = MapElevation (cell_x, cell_y + 1);
  y3 = MapElevation (cell_x + 1, cell_y + 1);
  if (dx < dy) {
    c = y2 - y0; 
    b = y3 - y2; 
    a = y0;
  } else {
    c = y3 - y1; 
    b = y1 - y0; 
    a = y0;
  }
  return (a + b * dx + c * dy);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

int MapSize ()
{

  return MAP_AREA;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLvector MapPosition (int x, int y)
{
  
  x = CLAMP (x, 0, MAP_AREA);
  y = CLAMP (y, 0, MAP_AREA);
  return map[x][y].position;

}

/*-----------------------------------------------------------------------------
This is a little goofy.  There are several different texture layers on the 
terrain, but the lowest one (grass) is always opaque. So, we leave that 
entry out of the array.  This means we have to subtract 1 from the given index.
Confusing, but this grid is big and there is no sense in storing half a million
redundant values.
-----------------------------------------------------------------------------*/

float MapLayer (int x, int y, int layer)
{

  //the base layer is always opaque
  if (layer == LAYER_GRASS)
    return 1.0f;
  layer -= 1;
  x = CLAMP (x, 0, MAP_AREA);
  y = CLAMP (y, 0, MAP_AREA);
  return (float)map[x][y].layer[layer] / 255.0f;

}

/*-----------------------------------------------------------------------------
How far is the given point from the camera?  These values are updated during 
TerrainUpdate () and are rarely 100% accurate.  These are used when calculating
detail on the terrain.
-----------------------------------------------------------------------------*/

float MapDistance (int x, int y)
{
  
  x = CLAMP (x, 0, MAP_AREA);
  y = CLAMP (y, 0, MAP_AREA);
  return map[x][y].distance;

}

/*-----------------------------------------------------------------------------
The the lighting color of the given point.
-----------------------------------------------------------------------------*/

GLrgba MapLight (int x, int y)
{
  
  x = CLAMP (x, 0, MAP_AREA);
  y = CLAMP (y, 0, MAP_AREA);
  return map[x][y].light;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void MapTerm (void)
{

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void MapUpdate (void)
{

  int       x;
  int       samples;
  int       start, end, step;
  GLvector  light;
  GLrgba    ambient, sun, shadow;
  float     dot;
  float     top;
  float     drop;
  float     shade;
  cell*     c;
  unsigned  update_end;

  light = WorldLightVector ();
  sun = WorldLightColor ();
  ambient = WorldAmbientColor ();
  shadow = glRgbaMultiply (ambient, glRgba (0.3f, 0.5f, 0.9f));
  if (light.x > 0.0f) {
    start = MAP_AREA;
    end = -1;
    step = -1;
  } else {
    start = 0;
    end = MAP_SIZE;
    step = 1;
  }
  if (light.x == 0.0f)
    drop = 9999999.0f;
  else
    drop = light.y / light.x;
  drop = ABS (drop);
  update_end = GetTickCount () + UPDATE_TIME;
  while (GetTickCount () < update_end) {
    //pass over the map (either east to west or vice versa) and see which points
    //are being hit with sunlight.  
    for (x = start; x != end; x += step) {
      c = &map[x][scan_y]; 
      c->distance = glVectorLength (glVectorSubtract (c->position, CameraPosition ()));
      c->distance /= FAR_VIEW;
      c->distance = CLAMP (c->distance, 0.0f, 1.0f);
      if (x == start) { //first point is always in sunlight
        top = c->position.y;
        c->shadow = false;
      } else { 
        top -= drop;
        if (c->position.y > top) { //is this point high enough to be out of the shadow?
          c->shadow = false;
          top = c->position.y;
        } else { //nope!
          c->shadow = true;
        }
      }
      dot = glVectorDotProduct (light, c->normal);
      dot = CLAMP (dot, 0.0f, 1.0f);
      samples = 0;
      shade = 0.0f;
      //blend this shadow with adjoining ones to soften the edges of shadows.
      //totally not needed, and it slows this down a bit.  You only need this
      //if the terrain is going to be viewed in close a lot.
      for (int xx = -1; xx <= 1; xx++) {
        for (int yy = -1; yy <= 1; yy++) {
          if (map[LIMIT((x + xx))][LIMIT((scan_y + yy))].shadow)
            shade += 1.0f;
          samples++;
        }
      }
      //finally! We know how much light is hitting this point and if it is in shadow
      //now figure out what color this point is
      c->light = glRgbaInterpolate (
        glRgbaAdd (ambient, glRgbaScale (sun, dot)),
        glRgbaAdd (shadow, glRgbaScale (ambient, dot)),
        shade / (float)samples);
    }
    scan_y = (scan_y + 1) % MAP_SIZE;
  }


}


  
