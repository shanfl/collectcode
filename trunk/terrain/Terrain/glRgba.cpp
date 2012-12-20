/*-----------------------------------------------------------------------------

  glRgba.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------

  Functions for dealing with RGBA color values.

-----------------------------------------------------------------------------*/

#include <windows.h>
#include <gl\gl.h>

#include "math.h"
#include "glTypes.h"

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgbaInterpolate (GLrgba c1, GLrgba c2, float delta)
{

  GLrgba     result;

  result.red = MathInterpolate (c1.red, c2.red, delta);
  result.green = MathInterpolate (c1.green, c2.green, delta);
  result.blue = MathInterpolate (c1.blue, c2.blue, delta);
  result.alpha = MathInterpolate (c1.alpha, c2.alpha, delta);
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgbaAdd (GLrgba c1, GLrgba c2)
{

  GLrgba     result;

  result.red = c1.red + c2.red;
  result.green = c1.green + c2.green;
  result.blue = c1.blue + c2.blue;
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgbaSubtract (GLrgba c1, GLrgba c2)
{

  GLrgba     result;

  result.red = c1.red - c2.red;
  result.green = c1.green - c2.green;
  result.blue = c1.blue - c2.blue;
  return result;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgbaMultiply (GLrgba c1, GLrgba c2)
{

  GLrgba     result;

  result.red = c1.red * c2.red;
  result.green = c1.green * c2.green;
  result.blue = c1.blue * c2.blue;
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgbaScale (GLrgba c, float scale)
{

  c.red *= scale;
  c.green *= scale;
  c.blue *= scale;
  return c;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgba (float red, float green, float blue)
{

  GLrgba     result;

  result.red = red;
  result.green = green;
  result.blue = blue;
  result.alpha = 1.0f;
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgba (float red, float green, float blue, float alpha)
{

  GLrgba     result;

  result.red = red;
  result.green = green;
  result.blue = blue;
  result.alpha = alpha;
  return result;

}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgba (long c)
{

  GLrgba     result;

  result.red = (float)GetRValue (c) / 255.0f;
  result.green = (float)GetGValue (c) / 255.0f;
  result.blue = (float)GetBValue (c) / 255.0f;
  result.alpha = 1.0f;
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLrgba glRgba (float luminance)
{

  GLrgba     result;

  result.red = luminance;
  result.green = luminance;
  result.blue = luminance;
  result.alpha = 1.0f;
  return result;

}