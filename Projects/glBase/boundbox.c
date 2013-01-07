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
	File:		boundbox.c

	Function:	bounding box class - puts a bounding box "shell" around an
				object which can be used to test the object against other
				primitives, e.g. to perform inside/outside tests against a
				plane or frustum.
*/

#include "boundbox.h"
#include <limits.h>

/*
  Test a bounding box against a plane. Returns 0 if completely outside,
  1 if completely inside, 2 if intersected.

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
int bbox_isInsidePlane(bbox b, plane p)
{
	/* Assemble accept / reject points */
	vec3 accept, reject;
	int i;
	for (i = 0; i < 3; i++) {
		if (p->signbits & (8 << i)) {
			reject[i] = b->maxp[i];
			accept[i] = b->minp[i];
		}
		else {
			reject[i] = b->minp[i];
			accept[i] = b->maxp[i];
		}
	}

	/* If reject point is outside the clipping plane */
	if (!vec3_isInsidePlane(reject, p))
		return 0;
	/* If accept point is inside the clipping plane */
	if (vec3_isInsidePlane(accept, p))
		return 1;
	/* Otherwise intersected */
	return 2;
}

/*
  Test a bounding box against the sides of a frustum and set the clip flags
  accordingly. Take the clip flags for a parent box (e.g. for a parent BSP
  node) into consideration when doing this.
  Returns:  clip flags
     plane    index     clip flag
     -----    -----     ---------
    (completely inside    0x00)
    (completely outside   0x01)
     top        0         0x02
     bottom     1         0x04
     right      2         0x08
     left       3         0x10
     near       4         0x20
     far        5         0x40
*/
int bbox_isInsideFrustum(bbox b, fru f, int clipFlags)
{
	int i, side;

	/*
	 * If parent box is wholly inside the frustum
	 * there is no need to test again
	 */
	 if (!clipFlags)
		 return clipFlags;

	/* For each clip plane */
	for (i = 0; i < FRUSTUM_NUMPLANES; i++) {

		/* If parent box is wholly inside this frustum
		 * plane there is no need to test again
		 */
		if (!(clipFlags & (2 << i)))
			continue;

		side = bbox_isInsidePlane(b, &f->planes[i]);
		/* If box is wholly outside the clipping plane */
		if (!side) {
			/* Reject the bounding box completely */
			clipFlags = 1;
			break;
		}
		/* If box is wholly inside the clipping plane */
		if (side == 1) {
			/* Accept the bounding box - turn off the clip flag */
			clipFlags &= ~(2 << i);
		}
	}
	return clipFlags;
}

/*
  Test a bounding box against another bounding box.
  Returns:
    0 if completely outside
    1 if completely inside
	2 if boxes overlap
*/
int bbox_isInsideBox(bbox b, bbox other)
{
	if (b->maxp[0] < other->minp[0] ||
		b->minp[0] > other->maxp[0] ||
		b->maxp[1] < other->minp[1] ||
		b->minp[1] > other->maxp[1] ||
		b->maxp[2] < other->minp[2] ||
		b->minp[2] > other->maxp[2])
		return 0;

	if (b->maxp[0] <= other->maxp[0] &&
		b->minp[0] >= other->minp[0] &&
		b->maxp[1] <= other->maxp[1] &&
		b->minp[1] >= other->minp[1] &&
		b->maxp[2] <= other->maxp[2] &&
		b->minp[2] >= other->minp[2])
		return 1;

	return 2;
}

/*
  Test a bounding box against a directed line segment from p0 to p1

  Returns
  
	false if no intersection -	line segment is parallel to and outside box,
								or stops before box, or starts after box.
	true if intersection -		some or all of line segment is inside box.

  If intersection
  
	s0 and s1 are set to the fractional distance along the line segment to
	the intersection points. If s0 < 0, the line segment starts inside the
	box, if s1 > 1, the line segment ends inside the box.
*/
bool bbox_intersectLine(bbox b, vec3 p0, vec3 p1, float *s0, float *s1)
{
	int i;
	float d, t0, t1, temp;

	*s0 = -(float)INT_MAX;
	*s1 = (float)INT_MAX;

	/* For each dimension */
	for (i = 0; i < 3; i++) {
		d = p1[i] - p0[i];
		/* Check parallel */
		if (d == 0.0f) {
			/* Check outside */
			if (p0[i] < b->minp[i] || p0[i] > b->maxp[i])
				/* Line is parallel to side and outside box */
				return false;
		}
		else {
			/* Not parallel - get intersections */
			t0 = (b->minp[i] - p0[i]) / d;
			t1 = (b->maxp[i] - p0[i]) / d;
			if (t0 > t1) {
				temp = t0; t0 = t1; t1 = temp;
			}
			if (t0 > *s0)
				*s0 = t0;		/* biggest s near */
			if (t1 < *s1)
				*s1 = t1;		/* smallest s far */
			if (*s0 > *s1)
				return false;	/* line misses box */
			if (*s1 < 0.0f)
				return false;	/* line starts after box */
			if (*s0 > 1.0f)
				return false;	/* line ends before box */
		}
	}
	return true;
}
