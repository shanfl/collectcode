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
	File:		frustum.h

	Function:	Frustum class, provides a simple view frustum.
*/

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "plane.h"
#include "camera.h"

#define FRUSTUM_TOP			0
#define FRUSTUM_BOTTOM		1
#define FRUSTUM_RIGHT		2
#define FRUSTUM_LEFT		3
#define FRUSTUM_NEAR		4
#define FRUSTUM_FAR			5
#define FRUSTUM_NUMPLANES	6

typedef struct fru_str {
	struct plane_str planes[FRUSTUM_NUMPLANES];
	vec3 eyePt;
} *fru;

#define fru_topNormal(f)	(f->planes[FRUSTUM_TOP].v)
#define fru_bottomNormal(f)	(f->planes[FRUSTUM_BOTTOM].v)
#define fru_rightNormal(f)	(f->planes[FRUSTUM_RIGHT].v)
#define fru_leftNormal(f)	(f->planes[FRUSTUM_LEFT].v)
#define fru_nearNormal(f)	(f->planes[FRUSTUM_NEAR].v)
#define fru_farNormal(f)	(f->planes[FRUSTUM_FAR].v)

#define fru_topOffset(f)	(f->planes[FRUSTUM_TOP].d)
#define fru_bottomOffset(f)	(f->planes[FRUSTUM_BOTTOM].d)
#define fru_rightOffset(f)	(f->planes[FRUSTUM_RIGHT].d)
#define fru_leftOffset(f)	(f->planes[FRUSTUM_LEFT].d)
#define fru_nearOffset(f)	(f->planes[FRUSTUM_NEAR].d)
#define fru_farOffset(f)	(f->planes[FRUSTUM_FAR].d)

fru fru_create(cam c);
void fru_destroy(fru f);
void fru_init(fru f, cam c);

bool vec3_isInsideFrustum(vec3 v, fru f);

#endif /* FRUSTUM_H */
