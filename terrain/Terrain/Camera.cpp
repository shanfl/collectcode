/*-----------------------------------------------------------------------------

  Camera.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------

  This module keeps tabs on the camera position, keeps it in bounds, and 
  handles some of the tricky rotations it has to do when moving around a given
  point on the terrain.  


-----------------------------------------------------------------------------*/

#define EYE_HEIGHT        2.0f
#define MAX_PITCH         85

#include <windows.h>
#include <math.h>

#include "glTypes.h"
#include "ini.h"
#include "macro.h"
#include "map.h"
#include "math.h"
#include "pointer.h"

static GLvector     angle;
static GLvector     position;
static float        distance;
static float        boost;
static float        movement;
static bool         moving;

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CameraYaw (float delta)
{

  moving = true;
  angle.y -= delta;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CameraPitch (float delta)
{

  moving = true;
  angle.x -= delta;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CameraPan (float delta)
{

  float           move_x, move_y;

  moving = true;
  move_x = (float)sin (-angle.y * DEGREES_TO_RADIANS) / 10.0f;
  move_y = (float)cos (-angle.y * DEGREES_TO_RADIANS) / 10.0f;
  position.x -= move_y * delta;
  position.z -= -move_x * delta;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CameraForward (float delta)
{

  float           move_x, move_y;

  moving = true;
  move_y = (float)sin (-angle.y * DEGREES_TO_RADIANS) / 10.0f;
  move_x = (float)cos (-angle.y * DEGREES_TO_RADIANS) / 10.0f;
  position.x -= move_y * delta;
  position.z -= move_x * delta;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CameraSelectionPitch (float delta)
{

  GLvector      center;
  float         pitch_to, yaw_to;
  float         horz_dist;
  float         total_dist;
  float         vert_dist;
  CPointer*     ptr;
  point         selected_cell;

  moving = true;
  ptr = (CPointer*)EntityFindType ("pointer", NULL);
  selected_cell = ptr->Selected ();
  delta *=  movement;
  if (selected_cell.x == -1 || selected_cell.y == -1) {
    angle.x -= delta;
    return;
  }
  center = MapPosition (selected_cell.x, selected_cell.y);
  vert_dist = position.y - center.y;
  yaw_to = MathAngle (center.x, center.z, position.x, position.z);
  horz_dist = MathDistance (center.x, center.z, position.x, position.z);
  total_dist = MathDistance (0.0f, 0.0f, horz_dist, vert_dist);
  pitch_to = MathAngle (vert_dist, 0.0f, 0.0f, -horz_dist);
  angle.x += delta;
  pitch_to += delta;
  angle.x -= MathAngleDifference (angle.x, pitch_to) / 15.0f;
  vert_dist = (float)sin (pitch_to * DEGREES_TO_RADIANS) * total_dist;
  horz_dist = (float)cos (pitch_to * DEGREES_TO_RADIANS) * total_dist;
  position.x = center.x - (float)sin (yaw_to * DEGREES_TO_RADIANS) * horz_dist;
  position.y = center.y + vert_dist;
  position.z = center.z - (float)cos (yaw_to * DEGREES_TO_RADIANS) * horz_dist;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CameraSelectionZoom (float delta)
{

  GLvector      center;
  GLvector      offset;
  float         total_dist;
  CPointer*     ptr;
  point         selected_cell;

  moving = true;
  ptr = (CPointer*)EntityFindType ("pointer", NULL);
  selected_cell = ptr->Selected ();
  delta *=  movement;
  if (selected_cell.x == -1 || selected_cell.y == -1) {
    angle.x -= delta;
    return;
  }
  center = MapPosition (selected_cell.x, selected_cell.y);
  offset = glVectorSubtract (position, center);
  total_dist = glVectorLength (offset);
  offset = glVectorNormalize (offset);
  total_dist += delta;
  offset = glVectorScale (offset, total_dist);
  position = glVectorAdd (center, offset);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CameraSelectionYaw (float delta)
{

  GLvector      center;
  float         yaw_to;
  float         horz_dist;
  float         total_dist;
  float         vert_dist;
  CPointer*     ptr;
  point         selected_cell;

  moving = true;
  ptr = (CPointer*)EntityFindType ("pointer", NULL);
  selected_cell = ptr->Selected ();
  delta *=  movement;
  if (selected_cell.x == -1 || selected_cell.y == -1) {
    angle.y -= delta;
    return;
  }
  center = MapPosition (selected_cell.x, selected_cell.y);
  vert_dist = position.y - center.y;
  yaw_to = MathAngle (center.x, center.z, position.x, position.z);
  horz_dist = MathDistance (center.x, center.z, position.x, position.z);
  total_dist = MathDistance (0.0f, 0.0f, horz_dist, vert_dist);
  angle.y -= MathAngleDifference (angle.y, -yaw_to + 180.0f) / 15.0f;
  yaw_to += delta;
  angle.y -= delta;
  position.x = center.x - (float)sin (yaw_to * DEGREES_TO_RADIANS) * horz_dist;
  position.y = center.y + vert_dist;
  position.z = center.z - (float)cos (yaw_to * DEGREES_TO_RADIANS) * horz_dist;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLvector CameraPosition (void)		
{
 
  return position;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CameraPositionSet (GLvector new_pos)		
{
 
  float     limit;
  float     elevation;

  limit = (float)MapSize ();
  position = new_pos;
  position.x = CLAMP (position.x, -limit, limit);
  position.y = CLAMP (position.y, -512.0f, 512.0f);
  position.z = CLAMP (position.z, -limit, limit);
  elevation = MapElevation (position.x, position.z) + EYE_HEIGHT;
  position.y = MAX (elevation, position.y);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

GLvector CameraAngle (void)		
{

  return angle;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CameraAngleSet (GLvector new_angle)		
{

  angle = new_angle;
  angle.x = CLAMP (angle.x, -80.0f, 80.0f);

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CameraInit (void)		
{

  angle = IniVector ("CameraAngle");
  position = IniVector ("CameraPosition");

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CameraUpdate (void)		
{

  float     limit;
  float     elevation;

  if (moving) 
    movement *= 1.1f;
  else
    movement = 0.0f;
  movement = CLAMP (movement, 0.01f, 1.0f);
  limit = (float)MapSize () * 1.5f;
  position.x = CLAMP (position.x, -limit, limit);
  position.y = CLAMP (position.y, -512.0f, 512.0f);
  position.z = CLAMP (position.z, -limit, limit);
  elevation = MapElevation (position.x, position.z) + EYE_HEIGHT;
  position.y = MAX (elevation, position.y);
  angle.x = CLAMP (angle.x, -MAX_PITCH, MAX_PITCH);
  moving = false;
 
}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void CameraTerm (void)		
{

  //just store our most recent position in the ini
  IniVectorSet ("CameraAngle", angle);
  IniVectorSet ("CameraPosition", position);
 
}
