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
	File:		vector.c

	Function:	Vector class
*/

#include "vector.h"

static void vec3_getRotationMatrix(vec3 v, float* out, float radians);

float vec3_getPhi(vec3 v)
{
	float x = vec3_dot(v, v);
	if (x < 0.0001f)
		return 0.0f;
	x = v[2]/sqrt(x);
	if (x < -1.0f)
		x = -1.0f;
	else if (x > 1.0f)
		x = 1.0f;
	return acos(x);
}

float vec3_getTheta(vec3 v)
{
	float theta;
	float x = v[0]*v[0] + v[1]*v[1];
	if (x < 0.0001f)
		return 0.0f;
	x = v[0]/sqrt(x);
	if (x < -1.0f)
		x = -1.0f;
	else if (x > 1.0f)
		x = 1.0f;
	theta = acos(x);

	/* Convert to 0->2M_PI from -M_PI->M_PI */
	if (v[1] < 0.0f)
		theta = 2.0f*M_PI - theta;

	return theta;
}

void vec3_setPhi(vec3 v, float phi)
{
	float len, theta;

	while (phi >= 2.0f*M_PI)
		phi -= 2.0f*M_PI;
	while(phi < 0.0f)
		phi += 2.0f*M_PI;

	theta = vec3_getTheta(v);
	len = vec3_getLen(v);

	v[0] = len*sin(phi)*cos(theta);
	v[1] = len*sin(phi)*sin(theta);
	v[2] = len*cos(phi);
}

void vec3_setTheta(vec3 v, float theta)
{
	float len, phi;

	while (theta >= 2.0f*M_PI)
		theta -= 2.0f*M_PI;
	while(theta < 0.0f)
		theta += 2.0f*M_PI;

	phi = vec3_getPhi(v);
	len = vec3_getLen(v);

	v[0] = len*sin(phi)*cos(theta);
	v[1] = len*sin(phi)*sin(theta);
}

void vec3_rotate(vec3 v, float addTheta, float addPhi)
{
	float phi = vec3_getPhi(v) + addPhi;
	float theta = vec3_getTheta(v) + addTheta;
	float len = vec3_getLen(v);

	while (phi >= 2.0f*M_PI)
		phi -= 2.0f*M_PI;
	while(phi < 0.0f)
		phi += 2.0f*M_PI;
	while (theta >= 2.0f*M_PI)
		theta -= 2.0f*M_PI;
	while(theta < 0.0f)
		theta += 2.0f*M_PI;

	v[0] = len*sin(phi)*cos(theta);
	v[1] = len*sin(phi)*sin(theta);
	v[2] = len*cos(phi);
}

/*
 * TODO: If you look at the usage of this function, radians is often
 * the same for each call, so optimise this to not recalc m if radians
 * hasn't changed.
 */
void vec3_axisRotate(vec3 axis, vec3 target, float radians)
{
	float m[9];
	vec3 t;
	int i;

	vec3_getRotationMatrix(axis, m, radians);
	vec3_cpy(target, t);

#define M(row,col)	m[row*3 + col]

	for (i = 0; i < 3; i++)
		target[i] = t[0]*M(0,i) + t[1]*M(1,i) + t[2]*M(2,i);

#undef M
}

static void vec3_getRotationMatrix(vec3 v, float* out, float radians)
{
  /* This function contributed by Erich Boleyn (erich@uruk.org) */
  /* This function used from the Mesa OpenGL code (matrix.c)  */
  float invlen, s, c;
  float vx, vy, vz, xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

  s = sin(radians);
  c = cos(radians);

  invlen = 1.0f / vec3_getLen(v);

  vx = v[0] * invlen;
  vy = v[1] * invlen;
  vz = v[2] * invlen;

#define M(row,col)  out[row*3 + col]

 /*
  *  Arbitrary axis rotation matrix.
  *
  *  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
  *  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
  *  (which is about the X-axis), and the two composite transforms
  *  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
  *  from the arbitrary axis to the X-axis then back.  They are
  *  all elementary rotations.
  *
  *  Rz' is a rotation about the Z-axis, to bring the axis vector
  *  into the x-z plane.  Then Ry' is applied, rotating about the
  *  Y-axis to bring the axis vector parallel with the X-axis.  The
  *  rotation about the X-axis is then performed.  Ry and Rz are
  *  simply the respective inverse transforms to bring the arbitrary
  *  axis back to it's original orientation.  The first transforms
  *  Rz' and Ry' are considered inverses, since the data from the
  *  arbitrary axis gives you info on how to get to it, not how
  *  to get away from it, and an inverse must be applied.
  *
  *  The basic calculation used is to recognize that the arbitrary
  *  axis vector (x, y, z), since it is of unit length, actually
  *  represents the sines and cosines of the angles to rotate the
  *  X-axis to the same orientation, with theta being the angle about
  *  Z and phi the angle about Y (in the order described above)
  *  as follows:
  *
  *  cos ( theta ) = x / sqrt ( 1 - z^2 )
  *  sin ( theta ) = y / sqrt ( 1 - z^2 )
  *
  *  cos ( phi ) = sqrt ( 1 - z^2 )
  *  sin ( phi ) = z
  *
  *  Note that cos ( phi ) can further be inserted to the above
  *  formulas:
  *
  *  cos ( theta ) = x / cos ( phi )
  *  sin ( theta ) = y / cos ( phi )
  *
  *  ...etc.  Because of those relations and the standard trigonometric
  *  relations, it is pssible to reduce the transforms down to what
  *  is used below.  It may be that any primary axis chosen will give the
  *  same results (modulo a sign convention) using thie method.
  *
  *  Particularly nice is to notice that all divisions that might
  *  have caused trouble when parallel to certain planes or
  *  axis go away with care paid to reducing the expressions.
  *  After checking, it does perform correctly under all cases, since
  *  in all the cases of division where the denominator would have
  *  been zero, the numerator would have been zero as well, giving
  *  the expected result.
  */

  xx = vx * vx;
  yy = vy * vy;
  zz = vz * vz;
  xy = vx * vy;
  yz = vy * vz;
  zx = vz * vx;
  xs = vx * s;
  ys = vy * s;
  zs = vz * s;
  one_c = 1.0F - c;

  M(0,0) = (one_c * xx) + c;
  M(1,0) = (one_c * xy) - zs;
  M(2,0) = (one_c * zx) + ys;

  M(0,1) = (one_c * xy) + zs;
  M(1,1) = (one_c * yy) + c;
  M(2,1) = (one_c * yz) - xs;

  M(0,2) = (one_c * zx) - ys;
  M(1,2) = (one_c * yz) + xs;
  M(2,2) = (one_c * zz) + c;

#undef M
}
