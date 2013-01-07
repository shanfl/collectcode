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
	File:		viewpoint.h

	Function:	Viewpoint class, defines a view origin &
				coordinate system
*/

#ifndef VIEWPOINT_H
#define VIEWPOINT_H

#include "vector.h"

typedef struct vpt_str {
	vec3 origin;
	vec3 orientation;
	vec3 upVector;
	vec3 leftVector;
} *vpt;

#define vpt_translate(v,a)			vec3_add(v->origin,a,v->origin)

/* Translate using 3 scalars */
#define vpt_translateS(v,x,y,z)		{v->origin[0] += x;\
									 v->origin[1] += y;\
									 v->origin[2] += z;}

/*
 * Translate a given distance in a given direction,
 * assume direction is normalized
 */
#define vpt_translateBy(v,dir,dst)	{\
	vec3 a; vec3_mulS(dir,dst,a); vec3_add(v->origin,a,v->origin);}

vpt vpt_create(void);
void vpt_default(vpt v);
void vpt_destroy(vpt v);
void vpt_init(vpt v, vec3 origin, vec3 orientation, vec3 upVector);
void vpt_rotate(vpt v, vec3 axis, float radians);
void vpt_yaw(vpt v, float radians);
void vpt_pitch(vpt v, float radians);
void vpt_roll(vpt v, float radians);
void vpt_getViewMatrix(vpt v, float out[3][3]);

#endif /* VIEWPOINT_H */
