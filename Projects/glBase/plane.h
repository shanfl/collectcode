/*
Copyright (C) 2000-2001 Adrian Welbourn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
/*
	File:		plane.h

	Function:	3D plane class.
*/

#ifndef PLANE_H
#define PLANE_H

#include "vector.h"

/*

  We use Hearn & Baker's definition of inside and outside.
  In a right handed system, the normal abc is directed from inside (behind)
  the plane to outside (in front of) the plane. The outside of the plane
  is the visible side.
  Polygons in the plane with counter-clockwise winding when viewed from
  outside the plane are only visible from outside the plane (front facing).
  Polygons in the plane with counter-clockwise winding when viewed from
  inside the plane are only visible from inside the plane (back facing).
                                 The plane equation is  ax + by + cz + d = 0.
       Points inside (behind) the plane are defined by  ax + by + cz + d < 0
  Points ouside (in front of) the plane are defined by  ax + by + cz + d > 0

*/

typedef struct plane_str {
	vec3 v;
	float d;
	unsigned char signbits;
} *plane;

#define plane_translate(p,v)	{p->d -= vec3_dot(p->v,v);}

void plane_setSignbits(plane p);

bool vec3_isInsidePlane(vec3 v, plane p);

#endif /* PLANE_H */
