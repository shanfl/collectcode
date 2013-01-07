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
	File:		camera.c

	Function:	Camera class - defines a viewpoint (origin & direction),
				field of view and far clipping plane.
*/

#include "camera.h"

static cam globalCamera;

void cam_setAspect(cam c, float aspect)
{
	float halfFovY = DEG2RAD(c->fovY / 2.0f);
	float halfFovX = atan(tan(halfFovY) * aspect);
	c->fovX = RAD2DEG(2.0f * halfFovX);
	c->aspect = aspect;
}

/*
 * If theres no global camera this creates it
 */
cam cam_create(void)
{
	cam c = (cam)malloc(sizeof(struct cam_str));
	if (c) {
		cam_default(c);
		if (!globalCamera)
			globalCamera = c;
	}
	return c;
}

void cam_default(cam c)
{
	if (c) {
		vpt_default(&c->viewPt);
		c->fovY = 75;
		c->farPlane = 200;
		c->nearPlane = 1;
		cam_setAspect(c, 1.33333f);
	}
}

void cam_destroy(cam c)
{
	if (c) {
		if (c == globalCamera)
			globalCamera = NULL;
		free(c);
	}
}

/*
 * Get the current global camera - if there isn't one, create one
 */
cam cam_getGlobalCamera(void)
{
    if (!globalCamera)
		cam_create();
	return globalCamera;
}

/*
 * Replace the current global camera
 */
cam cam_setGlobalCamera(cam c)
{
	cam old = globalCamera;
	globalCamera = c;
	return old;
}
