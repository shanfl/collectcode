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
	File:		viewpoint.c

	Function:	Viewpoint class, defines a view origin &
				coordinate system
*/

#include "viewpoint.h"

/*
 * We want to guarantee an orthogonal right handed coordinate system
 * with the orientation looking along the negative z axis
 */
static void vpt_setOrtho(vpt v)
{
	vec3_cross(v->upVector, v->orientation, v->leftVector);
	vec3_cross(v->orientation, v->leftVector, v->upVector);
	vec3_norm(v->orientation);
	vec3_norm(v->upVector);
	vec3_norm(v->leftVector);
}

vpt vpt_create(void)
{
	vpt v = (vpt)malloc(sizeof(struct vpt_str));
	if (v)
		vpt_default(v);
	return v;
}

void vpt_default(vpt v)
{
	if (v) {
		/* Set up Quake type view coordinate system by default */
		vec3 origin = {0, 0, 0};
		vec3 orientation = {1, 0, 0};
		vec3 upVector = {0, 0, 1};
		vpt_init(v, origin, orientation, upVector);
	}
}

void vpt_destroy(vpt v)
{
	if (v)
		free(v);
}

void vpt_init(vpt v, vec3 origin, vec3 orientation, vec3 upVector)
{
	vec3_cpy(origin, v->origin);
	vec3_cpy(orientation, v->orientation);
	vec3_cpy(upVector, v->upVector);
	vpt_setOrtho(v);
}

void vpt_rotate(vpt v, vec3 axis, float radians)
{
	/* Arbitrary axis rotation */
	vec3_axisRotate(axis, v->orientation, radians);
	vec3_axisRotate(axis, v->upVector, radians);
	vpt_setOrtho(v);
}

void vpt_yaw(vpt v, float radians)
{
	/* Spin left and forward vectors around the up vector */
	vec3_axisRotate(v->upVector, v->orientation, radians);
	vpt_setOrtho(v);
}

void vpt_pitch(vpt v, float radians)
{
	/* Spin up and forward vectors around the right vector. */
	vec3_axisRotate(v->leftVector, v->orientation, -radians);
	vec3_axisRotate(v->leftVector, v->upVector, -radians);
	vpt_setOrtho(v);
}

void vpt_roll(vpt v, float radians)
{
	/* Spin up and left vectors around the backwards vector. */
	vec3_axisRotate(v->orientation, v->upVector, -radians);
	vpt_setOrtho(v);
}

void vpt_getViewMatrix(vpt v, float out[3][3])
{
	vec3_neg(v->leftVector,	 out[0]);
	vec3_cpy(v->upVector,	 out[3]);
	vec3_neg(v->orientation, out[6]);
}
