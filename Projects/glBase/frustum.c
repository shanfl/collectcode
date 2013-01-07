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
	File:		frustum.c

	Function:	Frustum class, provides a simple view frustum.
*/

#include "frustum.h"

fru fru_create(cam c)
{
	fru f = (fru)malloc(sizeof(struct fru_str));
	if (f)
		fru_init(f, c);
	return f;
}

void fru_destroy(fru f)
{
	if (f)
		free(f);
}

void fru_init(fru f, cam c)
{
	int i;
	float a, b;
	vec3 farPt;

	/* Get camera position */
	vpt viewPt = &c->viewPt;
	vec3_cpy(viewPt->origin, f->eyePt);

	/* Get camera perspective */
	a = DEG2RAD(c->fovY / 2.0f);
	b = DEG2RAD(c->fovX / 2.0f);

	/* Get the plane normals */
	vec3_cpy(viewPt->upVector, fru_topNormal(f));
	vec3_axisRotate(viewPt->leftVector, fru_topNormal(f), -a);

	vec3_neg(viewPt->upVector, fru_bottomNormal(f));
	vec3_axisRotate(viewPt->leftVector, fru_bottomNormal(f), a);

	vec3_neg(viewPt->leftVector, fru_rightNormal(f));
	vec3_axisRotate(viewPt->upVector, fru_rightNormal(f), -b);

	vec3_cpy(viewPt->leftVector, fru_leftNormal(f));
	vec3_axisRotate(viewPt->upVector, fru_leftNormal(f), b);

	vec3_neg(viewPt->orientation, fru_nearNormal(f));
	vec3_cpy(viewPt->orientation, fru_farNormal(f));

	/*
	 * Now calculate the plane offsets from the normals and
	 * the eye position.
	 * TODO: Need an epsilon here?
	 */
	fru_topOffset(f)	= -vec3_dot(f->eyePt, fru_topNormal(f));
	fru_bottomOffset(f)	= -vec3_dot(f->eyePt, fru_bottomNormal(f));
	fru_rightOffset(f)	= -vec3_dot(f->eyePt, fru_rightNormal(f));
	fru_leftOffset(f)	= -vec3_dot(f->eyePt, fru_leftNormal(f));
	fru_nearOffset(f)	= -vec3_dot(f->eyePt, fru_nearNormal(f));

	/*
	 * For the far plane, find the point farDist away from the
	 * eye along the forward vector.
	 */
	vec3_mulS(viewPt->orientation, c->farPlane, farPt);
	vec3_add(farPt, f->eyePt, farPt);
	fru_farOffset(f)	= -vec3_dot(farPt, fru_farNormal(f));

	/*
	 * Make sure plane is normalised and sign bits are set correctly.
	 */
	for (i = 0; i < FRUSTUM_NUMPLANES; i++)
		plane_setSignbits(&f->planes[i]);
}

/*

  Test a point against the sides of the frustum. Returns false if outside,
  true if inside.

*/
bool vec3_isInsideFrustum(vec3 v, fru f)
{
	int i;

	/* For each clip plane */
	for (i = 0; i < FRUSTUM_NUMPLANES; i++) {
		/* If point is outside, reject the point */
		if (!vec3_isInsidePlane(v, &f->planes[i]))
			return false;
	}
	return true;
}
