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
	File:		plane.c

	Function:	3D plane class.
*/

#include "plane.h"

/*

  plane_setSignbits

  The first three signbits indicate whether the plane is axially aligned and
  are used to speed up the dot product calculation used to test if a point is
  inside the plane.

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

  The next three signbits encode the signs of the abc components of the plane
  normal, and are used to build accept/reject points to test if a bounding box
  is inside the plane, e.g.:-

       outside           inside
                   |
    Normal   <-----|
                   |

               |       |
               |       |
            min x     max x

  If       max x is outside the plane, the bounding box is rejected
  Else If  min x is inside  the plane, the bounding box is accepted
  Else     the bounding box is clipped by the plane

  i.e. If N.x < 0, max x is the reject point, min x is the accept point.


        inside           outside
                   |
                   |----->   Normal
                   |

               |       |
               |       |
            min x     max x

  If       min x is outside the plane, the bounding box is rejected
  Else If  max x is inside  the plane, the bounding box is accepted
  Else     the bounding box is clipped by the plane

  i.e. If N.x > 0, min x is the reject point, max x is the accept point.

  The same argument applies to the y and z components.

*/
void plane_setSignbits(plane p)
{
	float invlen = 1.0f / vec3_getLen(p->v);
	if (invlen != 1.0f) {
		vec3_mulS(p->v, invlen, p->v);
		p->d *= invlen;
	}
	p->signbits = 0;
	if (fabs(p->v[0]) == 1)			p->signbits  =  1;
	else if (fabs(p->v[1]) == 1)	p->signbits  =  2;
	else if (fabs(p->v[2]) == 1)	p->signbits  =  4;
	if (p->v[0] < 0)				p->signbits |=  8;
	if (p->v[1] < 0)				p->signbits |= 16;
	if (p->v[2] < 0)				p->signbits |= 32;
}

/*
 * Test a point against the plane. Returns false if outside, true if inside.
 */
bool vec3_isInsidePlane(vec3 v, plane p)
{
	int i;
	float planeEqVal = p->d;

	if (p->signbits & 7) {
		for (i = 0; i < 3; i++) {
			if (p->signbits & (1 << i)) {
				planeEqVal += (p->signbits & (8 << i)) ? -v[i] : v[i];
				break;
			}
		}
	}
	else
		planeEqVal += vec3_dot(p->v, v);

	return (planeEqVal < 0);
}
