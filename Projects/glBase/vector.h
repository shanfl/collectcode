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
	File:		vector.h

	Function:	Vector class
*/

#ifndef VECTOR_H
#define VECTOR_H

#include "general.h"

typedef float vec3[3];		/* x, y, z */
typedef float vec5[5];		/* x, y, z, s, t */

/* 3D vector vector methods */
#define vec3_cpy(a,b)		{b[0]=a[0];b[1]=a[1];b[2]=a[2];}
#define vec3_neg(a,b)		{b[0]=-a[0];b[1]=-a[1];b[2]=-a[2];}
#define vec3_add(a,b,c)		{c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define vec3_sub(a,b,c)		{c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];}
#define vec3_mul(a,b,c)		{c[0]=a[0]*b[0];c[1]=a[1]*b[1];c[2]=a[2]*b[2];}
#define vec3_div(a,b,c)		{c[0]=a[0]/b[0];c[1]=a[1]/b[1];c[2]=a[2]/b[2];}
#define vec3_dot(a,b)		(a[0]*b[0]+a[1]*b[1]+a[2]*b[2])
#define vec3_cross(a,b,c)	{c[0] = a[1]*b[2] - a[2]*b[1];\
							 c[1] = a[2]*b[0] - a[0]*b[2];\
							 c[2] = a[0]*b[1] - a[1]*b[0];}

/* 3D Vector Scalar methods */
#define vec3_addS(a,b,c)	{c[0]=a[0]+(b);c[1]=a[1]+(b);c[2]=a[2]+(b);}
#define vec3_subS(a,b,c)	{c[0]=a[0]-(b);c[1]=a[1]-(b);c[2]=a[2]-(b);}
#define vec3_mulS(a,b,c)	{c[0]=a[0]*(b);c[1]=a[1]*(b);c[2]=a[2]*(b);}
#define vec3_divS(a,b,c)	{float d=1.0f/(b); c[0]=a[0]*d;c[1]=a[1]*d;c[2]=a[2]*d;}

/* 3D Vector methods */
#define vec3_getLen(a)		(float)sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2])
#define vec3_norm(a)		{float b=1.0f/(float)sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);\
							 a[0]=a[0]*b;a[1]=a[1]*b;a[2]=a[2]*b;}

/*
 * Spherical coordinates, theta is angle from x axis of projection on x-y plane,
 * phi is angle from z-axis (see Hearn & Baker).
 */
float vec3_getPhi(vec3 v);
float vec3_getTheta(vec3 v);
void vec3_setPhi(vec3 v, float phi);
void vec3_setTheta(vec3 v, float theta);
void vec3_rotate(vec3 v, float addTheta, float addPhi);

/* Arbitrary-axis rotation - rotate target around axis by the specified angle */
void vec3_axisRotate(vec3 axis, vec3 target, float radians);

#endif /* VECTOR_H */
