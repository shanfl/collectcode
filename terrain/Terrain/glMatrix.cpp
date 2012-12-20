/*-----------------------------------------------------------------------------

  glMatrix.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------
  
  Functions useful for manipulating the Matrix struct

-----------------------------------------------------------------------------*/


#define M(e,x,y)                (e.elements[x][y])

#include <math.h>

#include "macro.h"
#include "glTypes.h"

static float      identity[4][4] = 
{
  {1.0f, 0.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 0.0f, 1.0f},
};


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void* glMatrixCreate (void)
{

  GLmatrix*       m;
  int             x;
  int             y;

  m = new GLmatrix;
  for (x = 0; x < 4; x++) {
    for (y = 0; y < 4; y++) {
      m -> elements[x][y] = identity[x][y];
    }
  }
  return (void*)m;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLmatrix glMatrixIdentity (void)
{

  GLmatrix        m;
  int             x;
  int             y;

  for (x = 0; x < 4; x++) {
    for (y = 0; y < 4; y++) {
      M(m, x, y) = identity[x][y];
    }
  }
  return m;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void glMatrixElementsSet (GLmatrix* m, float* in)
{

  m -> elements[0][0] = in[0];
  m -> elements[0][1] = in[1];
  m -> elements[0][2] = in[2];
  m -> elements[0][3] = in[3];

  m -> elements[1][0] = in[4];
  m -> elements[1][1] = in[5];
  m -> elements[1][2] = in[6];
  m -> elements[1][3] = in[7];

  m -> elements[2][0] = in[8];
  m -> elements[2][1] = in[9];
  m -> elements[2][2] = in[10];
  m -> elements[2][3] = in[11];
  
  m -> elements[3][0] = in[12];
  m -> elements[3][1] = in[13];
  m -> elements[3][2] = in[14];
  m -> elements[3][3] = in[15];

}

/*---------------------------------------------------------------------------
A matrix multiplication (dot product) of two 4x4 matrices.
---------------------------------------------------------------------------*/

GLmatrix glMatrixMultiply (GLmatrix a, GLmatrix b)
{

  GLmatrix        result;
  
  M(result, 0,0) = M(a, 0,0) * M(b, 0, 0) + M(a, 1,0) * M(b, 0, 1) + M(a, 2,0) * M(b, 0, 2);
  M(result, 1,0) = M(a, 0,0) * M(b, 1, 0) + M(a, 1,0) * M(b, 1, 1) + M(a, 2,0) * M(b, 1, 2);
  M(result, 2,0) = M(a, 0,0) * M(b, 2, 0) + M(a, 1,0) * M(b, 2, 1) + M(a, 2,0) * M(b, 2, 2);
  M(result, 3,0) = M(a, 0,0) * M(b, 3, 0) + M(a, 1,0) * M(b, 3, 1) + M(a, 2,0) * M(b, 3, 2) + M(a, 3,0);
  
  M(result, 0,1) = M(a, 0,1) * M(b, 0, 0) + M(a, 1,1) * M(b, 0, 1) + M(a, 2,1) * M(b, 0, 2);
  M(result, 1,1) = M(a, 0,1) * M(b, 1, 0) + M(a, 1,1) * M(b, 1, 1) + M(a, 2,1) * M(b, 1, 2);
  M(result, 2,1) = M(a, 0,1) * M(b, 2, 0) + M(a, 1,1) * M(b, 2, 1) + M(a, 2,1) * M(b, 2, 2);
  M(result, 3,1) = M(a, 0,1) * M(b, 3, 0) + M(a, 1,1) * M(b, 3, 1) + M(a, 2,1) * M(b, 3, 2) + M(a, 3,1);

  M(result, 0,2) = M(a, 0,2) * M(b, 0, 0) + M(a, 1,2) * M(b, 0, 1) + M(a, 2,2) * M(b, 0, 2);
  M(result, 1,2) = M(a, 0,2) * M(b, 1, 0) + M(a, 1,2) * M(b, 1, 1) + M(a, 2,2) * M(b, 1, 2);
  M(result, 2,2) = M(a, 0,2) * M(b, 2, 0) + M(a, 1,2) * M(b, 2, 1) + M(a, 2,2) * M(b, 2, 2);
  M(result, 3,2) = M(a, 0,2) * M(b, 3, 0) + M(a, 1,2) * M(b, 3, 1) + M(a, 2,2) * M(b, 3, 2) + M(a, 3,2);
  return result;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLvector glMatrixTransformPoint (GLmatrix m, GLvector in)
{

  GLvector              out;

  out.x = M(m,0,0) * in.x + M(m,1,0) * in.y + M(m,2,0) * in.z + M(m,3,0);
  out.y = M(m,0,1) * in.x + M(m,1,1) * in.y + M(m,2,1) * in.z + M(m,3,1);
  out.z = M(m,0,2) * in.x + M(m,1,2) * in.y + M(m,2,2) * in.z + M(m,3,2);
  return out;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLmatrix glMatrixRotate (GLmatrix m, float theta, float x, float y, float z)
{

  GLmatrix              r;
  float                 length;
  float                 s, c, t;
  GLvector              in;

  theta *= DEGREES_TO_RADIANS;
  r = glMatrixIdentity ();
  length = (float)sqrt (x * x + y * y + z * z); 
  if (length < 0.00001f)
    return m;
  x /= length;
  y /= length;
  z /= length;
  s = (float)sin (theta);
  c = (float)cos (theta);
  t = 1.0f - c;  
 
  in.x = in.y = in.z = 1.0f;
  M(r, 0,0) = t*x*x + c;
  M(r, 1,0) = t*x*y - s*z;
  M(r, 2,0) = t*x*z + s*y;
  M(r, 3,0) = 0;

  M(r, 0,1) = t*x*y + s*z;
  M(r, 1,1) = t*y*y + c;
  M(r, 2,1) = t*y*z - s*x;
  M(r, 3,1) = 0;

  M(r, 0,2) = t*x*z - s*y;
  M(r, 1,2) = t*y*z + s*x;
  M(r, 2,2) = t*z*z + c;
  M(r, 3,2) = 0;

  m = glMatrixMultiply (m, r);
  return m;

}
