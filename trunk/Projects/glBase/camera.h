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
	File:		camera.h

	Function:	Camera class - defines a viewpoint (origin & direction),
				field of view and far clipping plane.
*/

#ifndef CAMERA_H
#define CAMERA_H

#include "general.h"
#include "viewpoint.h"

typedef struct cam_str {
	struct vpt_str viewPt;
	float fovY;
	float fovX;
	float aspect;
	float farPlane;
	float nearPlane;
} *cam;

void cam_setAspect(cam c, float aspect);
cam cam_create(void);
void cam_default(cam c);
void cam_destroy(cam c);
cam cam_getGlobalCamera(void);
cam cam_setGlobalCamera(cam c);

#endif /* CAMERA_H */
