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
	File:		boundbox.h

	Function:	bounding box class - puts a bounding box "shell" around an
				object which can be used to test the object against other
				primitives, e.g. to perform inside/outside tests against a
				plane or frustum.
*/

#ifndef BOUNDBOX_H
#define BOUNDBOX_H

#include "frustum.h"

typedef struct bbox_str {
	vec3 minp, maxp;
} *bbox;

int  bbox_isInsidePlane(bbox b, plane p);
int  bbox_isInsideFrustum(bbox b, fru f, int clipFlags);
int  bbox_isInsideBox(bbox b, bbox other);
bool bbox_intersectLine(bbox b, vec3 p0, vec3 p1, float *s0, float *s1);

#endif /* BOUNDBOX_H */
