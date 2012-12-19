/**
 * RglPrims.cpp
 * Copyright 2000 by Mark B. Allan
 * 
 * GL counterparts to RenderMan quadric primitives.
 * Be aware of handedness switch.
 * 
 * Put these calls in display lists! There was no attempt made at 
 * efficiency - each call has many calls to sin() and new/delete.
 *
 * The RenderMan(R) Interface Procedures and RIB Protocol are
 * Copyright 1988, 1989, Pixar.  All rights reserved.
 * RenderMan(R) is a registered trademark of Pixar.
 */
#include "RglPrims.h"

#include <GL/gl.h>

typedef	float RglPoint[3];

#define glTexCoord2f_M0(a, b) glTexCoord2f(a, b)
//#define glTexCoord2f_M1(a, b) glMultiTexCoord2fARB(GL_TEXTURE1_ARB, a, b)
#define glTexCoord2f_M1(a, b) 

/**
 * RiDisk clone
 */
//--------------------------------------------------------------------
void RglDisk (float height, float radius, float thetamax, int detail, bool dcomp)
{
	int 	zDensity;
	int		xyDensity;
	int		ixy;
	int		iz;
	float	x,y;
	float	rad;
	float	radStep;
	float	norm[3];
	
	//-- do some kludgy error checking
	if ( radius == 0.0f || thetamax == 0.0f  )
		return;
	if (thetamax >  360.0f)	thetamax = 360.0f;
	if (thetamax < -360.0f)	thetamax = -360.0f;
	
	//-- calculate tesselation
	xyDensity = abs((int)( (D2RAD(thetamax)/(2.0*M_PI))*(float)detail ));
	if(dcomp)
		zDensity = abs((int)((float)detail/4.0f));
	else
		zDensity = 2;
	if		(xyDensity < 4)		xyDensity = 4;
	else if	(xyDensity > 100)	xyDensity = 100;
	if		(zDensity < 2)		zDensity = 2;
	else if(zDensity > 100)		zDensity = 100;
	
	//-- make space for vertices
	RglPoint	**verts = new RglPoint*[zDensity+1];
	RglPoint	**norms = new RglPoint*[zDensity+1];
	for (iz = 0; iz < (zDensity+1); iz++)
	{
		norms[iz] = new RglPoint[xyDensity+1];
		verts[iz] = new RglPoint[xyDensity+1];
		for(ixy = 0; ixy < xyDensity+1; ixy++)
		{
			norms[iz][ixy][0] = 0.0f;
			norms[iz][ixy][1] = 0.0f;
			norms[iz][ixy][2] = 0.0f;
		}
	}
	
	//-- Calculate tip and base
	radStep	= (D2RAD(thetamax))/( (float)xyDensity );
	rad		= M_PI/2.0;
	for (ixy = 0; ixy < xyDensity+1; ixy++)
	{
		//-- verts for tip
		verts[0][ixy][0] = verts[0][ixy][1] = 0.0f;
		verts[0][ixy][2] = height;
		//-- verts for base
		x = sin (rad)*radius;	
		y = cos (rad)*radius;
		verts[zDensity-1][ixy][0] = x;
		verts[zDensity-1][ixy][1] = y;
		verts[zDensity-1][ixy][2] = height;
		if (ixy > 0)
		{
			if(thetamax < 0.0f)
				calcNormal	(norm, verts[0][ixy-1], verts[zDensity-1][ixy-1], verts[zDensity-1][ixy]);
			else
				calcNormal	(norm, verts[0][ixy-1], verts[zDensity-1][ixy], verts[zDensity-1][ixy-1]);
			norms[0][ixy][0] += norm[0];
			norms[0][ixy][1] += norm[1];
			norms[0][ixy][2] += norm[2];
			norms[0][ixy-1][0] += norm[0];
			norms[0][ixy-1][1] += norm[1];
			norms[0][ixy-1][2] += norm[2];
		}
		rad += radStep;
	}
	norms[0][0][0] += norms[0][xyDensity][0];
	norms[0][0][1] += norms[0][xyDensity][1];
	norms[0][0][2] += norms[0][xyDensity][2];
	norms[0][xyDensity][0] += norms[0][0][0];
	norms[0][xyDensity][1] += norms[0][0][1];
	norms[0][xyDensity][2] += norms[0][0][2];
	
	for(ixy = 0; ixy <  xyDensity+1; ixy++)
	{
		normalizeVec(norms[0][ixy]);
		norms[zDensity-1][ixy][0] = norms[0][ixy][0] ;
		norms[zDensity-1][ixy][1] = norms[0][ixy][1] ;
		norms[zDensity-1][ixy][2] = norms[0][ixy][2] ;
	}
		
	//-- Interpolate tesselation
	float	zDf = (float)zDensity-1;
	float	izf;
	for(iz = 1; iz < zDensity-1; iz++)
	{
		izf = (float)iz;
		for (ixy = 0; ixy < xyDensity+1; ixy++)
		{
			verts[iz][ixy][0] = verts[0][ixy][0]*((zDf-izf)/zDf) + verts[zDensity-1][ixy][0]*(izf/zDf);
			verts[iz][ixy][1] = verts[0][ixy][1]*((zDf-izf)/zDf) + verts[zDensity-1][ixy][1]*(izf/zDf);
			verts[iz][ixy][2] = verts[0][ixy][2]*((zDf-izf)/zDf) + verts[zDensity-1][ixy][2]*(izf/zDf);
			norms[iz][ixy][0] = norms[0][ixy][0];
			norms[iz][ixy][1] = norms[0][ixy][1];
			norms[iz][ixy][2] = norms[0][ixy][2];
		}
	}
	
	//-- Draw tip
	float s1, t1, t2, zd, xyd;
	zd	= zDensity-1.0;
	xyd	= xyDensity-1.0;
	
	s1 = 0.0;
	t1 = 1.0;	
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f_M0(s1, t1);
	glTexCoord2f_M1(s1, t1);
	glNormal3fv(norms[0][0]);
	glVertex3fv(verts[0][0]);
	for (ixy = xyDensity; ixy >= 0; ixy--)
	{
		s1 = ixy/xyd;
		glTexCoord2f_M0(s1, t1);
		glTexCoord2f_M1(s1, t1);
		glVertex3fv(verts[1][ixy]);
	}
	glEnd();
	//-- Draw the rest
	for(iz = 1; iz < zDensity-1; iz++)
	{
		t1 = (zd-iz)/zd;
		t2 = (zd-iz-1.0)/zd;
		glBegin(GL_QUAD_STRIP);
		izf = (float)iz;
		for (ixy = 0; ixy < xyDensity+1; ixy++)
		{
			s1 = ixy/xyd;
			glTexCoord2f_M0(s1, t2);
			glTexCoord2f_M1(s1, t2);
			glVertex3fv(verts[iz+1][ixy]);
			glTexCoord2f_M0(s1, t1);
			glTexCoord2f_M1(s1, t1);
			glVertex3fv(verts[iz][ixy]);
		}
		glEnd();
	}
	
	//-- cleanup
	for (iz = 0; iz < (zDensity+1); iz++)
	{
		delete []verts[iz];
		delete []norms[iz];
	}
	delete []verts;
	delete []norms;
}

/**
 * RiCone clone
 */
//--------------------------------------------------------------------
void RglCone (float height, float radius, float thetamax, int detail, bool dcomp)
{
	int 	zDensity;
	int		xyDensity;
	int		ixy;
	int		iz;
	float	x;
	float	y;
	float	rad;
	float	radStep;
	float	norm[3];
	
	//-- do some quick error checking
	if (!radius || !thetamax)	return;
	if (thetamax >  360.0f)		thetamax =  360.0f;
	if (thetamax < -360.0f)		thetamax = -360.0f;
	
	//-- calculate tesselation
	xyDensity = abs((int)( (D2RAD(thetamax)/(2.0*M_PI))*(float)detail ));
	if(dcomp)
		zDensity = abs((int)((height/radius)*((float)detail/5.0f)));
	else
		zDensity = 3;
	if 		(xyDensity < 4)		xyDensity = 4;
	else if	(xyDensity > 100)	xyDensity = 100;
	if		(zDensity < 2)		zDensity  = 2;
	else if	(zDensity > 100)	zDensity  = 100;
	
	//-- make space for vertices
	RglPoint	**verts = new RglPoint*[zDensity+1];
	RglPoint	**norms = new RglPoint*[zDensity+1];
	for (iz = 0; iz < (zDensity+1); iz++)
	{
		norms[iz] = new RglPoint[xyDensity+1];
		verts[iz] = new RglPoint[xyDensity+1];
		for(ixy = 0; ixy < xyDensity+1; ixy++)
		{
			norms[iz][ixy][0] = 0.0f;
			norms[iz][ixy][1] = 0.0f;
			norms[iz][ixy][2] = 0.0f;
		}
	}
	
	//-- Calculate tip and base
	radStep	= D2RAD(thetamax)/(float)xyDensity;
	rad		= M_PI/2.0;
	for (ixy = 0; ixy < xyDensity+1; ixy++)
	{
		//-- verts for tip
		verts[0][ixy][0] = verts[0][ixy][1] = 0.0f;
		verts[0][ixy][2] = height;
		//-- verts for base
		x = sin (rad)*radius;	
		y = cos (rad)*radius;
		verts[zDensity-1][ixy][0] = x;
		verts[zDensity-1][ixy][1] = y;
		verts[zDensity-1][ixy][2] = 0.0f;
		if (ixy > 0)
		{
			if(thetamax < 0.0f)
				calcNormal	(norm, verts[0][ixy-1], verts[zDensity-1][ixy-1], verts[zDensity-1][ixy]);
			else
				calcNormal	(norm, verts[0][ixy-1], verts[zDensity-1][ixy], verts[zDensity-1][ixy-1]);
			norms[0][ixy][0] += norm[0];
			norms[0][ixy][1] += norm[1];
			norms[0][ixy][2] += norm[2];
			norms[0][ixy-1][0] += norm[0];
			norms[0][ixy-1][1] += norm[1];
			norms[0][ixy-1][2] += norm[2];
		}
		rad += radStep;
	}
	if ( fabs(thetamax) > 350.0)
	{
		norms[0][0][0] += norms[0][xyDensity][0];
		norms[0][0][1] += norms[0][xyDensity][1];
		norms[0][0][2] += norms[0][xyDensity][2];
		norms[0][xyDensity][0] += norms[0][0][0];
		norms[0][xyDensity][1] += norms[0][0][1];
		norms[0][xyDensity][2] += norms[0][0][2];
	}
	for(ixy = 0; ixy <  xyDensity+1; ixy++)
	{
		normalizeVec(norms[0][ixy]);
		norms[zDensity-1][ixy][0] = norms[0][ixy][0] ;
		norms[zDensity-1][ixy][1] = norms[0][ixy][1] ;
		norms[zDensity-1][ixy][2] = norms[0][ixy][2] ;
	}
		
	//-- Interpolate tesselation
	float	zDf = (float)zDensity-1;
	float	izf;
	for(iz = 1; iz < zDensity-1; iz++)
	{
		izf = (float)iz;
		for (ixy = 0; ixy < xyDensity+1; ixy++)
		{
			verts[iz][ixy][0] = verts[0][ixy][0]*((zDf-izf)/zDf) + verts[zDensity-1][ixy][0]*(izf/zDf);
			verts[iz][ixy][1] = verts[0][ixy][1]*((zDf-izf)/zDf) + verts[zDensity-1][ixy][1]*(izf/zDf);
			verts[iz][ixy][2] = verts[0][ixy][2]*((zDf-izf)/zDf) + verts[zDensity-1][ixy][2]*(izf/zDf);
			norms[iz][ixy][0] = norms[0][ixy][0];
			norms[iz][ixy][1] = norms[0][ixy][1];
			norms[iz][ixy][2] = norms[0][ixy][2];
		}
	}
	
	float s1, s2, t1, t2, zd, xyd;
	zd	= zDensity-1.0;
	xyd	= xyDensity-1.0;
	//-- Draw tip
	t1 = 1.0;
	t2 = (zd-1.0)/zd;
	glBegin(GL_TRIANGLES);
	for (ixy = 0; ixy < xyDensity; ixy++)
	{
		s1 = ixy/xyd;
		s2 = (ixy+1.0)/xyd;
		
		glTexCoord2f_M0(s1, t1);
		glTexCoord2f_M1(s1, t1);
		glNormal3fv(norms[0][ixy]);
		glVertex3fv(verts[0][ixy]);
		
		glTexCoord2f_M0(s2, t2);
		glTexCoord2f_M1(s2, t2);
		glNormal3fv(norms[1][ixy+1]);
		glVertex3fv(verts[1][ixy+1]);
		
		glTexCoord2f_M0(s1, t2);
		glTexCoord2f_M1(s1, t2);
		glNormal3fv(norms[1][ixy]);
		glVertex3fv(verts[1][ixy]);
	}
	glEnd();
	//-- Draw the rest
	for(iz = 1; iz < zDensity-1; iz++)
	{
		t1 = (zd-iz)/zd;
		t2 = (zd-iz-1.0)/zd;
		glBegin(GL_QUAD_STRIP);
		izf = (float)iz;
		for (ixy = 0; ixy < xyDensity+1; ixy++)
		{
			s1 = ixy/xyd;
			
			glTexCoord2f_M0(s1, t2);
			glTexCoord2f_M1(s1, t2);
			glNormal3fv(norms[iz+1][ixy]);
			glVertex3fv(verts[iz+1][ixy]);
			
			glTexCoord2f_M0(s1, t1);
			glTexCoord2f_M1(s1, t1);
			glNormal3fv(norms[iz][ixy]);
			glVertex3fv(verts[iz][ixy]);
		}
		glEnd();
	}
	//-- Cleanup
	for (iz = 0; iz < (zDensity+1); iz++)
	{
		delete []verts[iz];
		delete []norms[iz];
	}
	delete []verts;
	delete []norms;
	
}

float	*vecReplace (float *vec, int index, float value) 
{ vec[index] = value; return vec; }
float	*copyVec(float *newVec, float *oldVec, int dim)	
{
	for (int i = 0; i < dim; i++)
		newVec[i] = oldVec[i];
	return newVec;
}
float	*normalizeVec (float *vec)
{
	float	length;
	length = sqrt( (vec[0]*vec[0]) + (vec[1]*vec[1]) + (vec[2]*vec[2]) );
	if (length == 0.0)
		length = 1.0;
	vec[0] /= length;
	vec[1] /= length;
	vec[2] /= length;
	return vec;
}	
float	*calcNormal (float *norm, float *vert1, float *vert2, float *vert3 )
{
	float	u1[3] = {	vert1[0] - vert2[0], 
						vert1[1] - vert2[1],
						vert1[2] - vert2[2] };
	float	u2[3] = {	vert2[0] - vert3[0],
						vert2[1] - vert3[1],
						vert2[2] - vert3[2] };
	norm[0] = u1[1]*u2[2] - u1[2]*u2[1];		
	norm[1] = u1[2]*u2[0] - u1[0]*u2[2];		
	norm[2] = u1[0]*u2[1] - u1[1]*u2[0];		
	float	length = sqrt( (norm[0]*norm[0]) + (norm[1]*norm[1]) + (norm[2]*norm[2]) );
	if (length == 0.0)
		length = 1.0;
	norm[0] /= length;
	norm[1] /= length;
	norm[2] /= length;
	return norm;
}

/**
 * RiCylinder clone
 */
//--------------------------------------------------------------------
void RglCylinder (float radius, float zmin, float zmax, float thetamax, int detail, bool dcomp)
{
	int 	zDensity;
	int		xyDensity;
	int		ixy;
	int		iz;
	float	x;
	float	y;
	float	rad;
	float	radStep;
	
	//-- do some quick error checking
	if (!radius || !thetamax)	return;
	if (thetamax >  360.0f)	thetamax = 360.0f;
	if (thetamax < -360.0f)	thetamax = -360.0f;
	
	//-- calculate tesselation
	xyDensity	= abs((int)( (D2RAD(thetamax)/(2.0*M_PI))*(float)detail ));
	if(dcomp)
		zDensity	= abs((int)(((zmax-zmin)/radius)*((float)detail/10.0f)));
	else
		zDensity = 3;
	if		(xyDensity < 4)		xyDensity = 4;
	else if	(xyDensity > 100)	xyDensity = 100;
	if		(zDensity < 2)		zDensity = 2;
	else if(zDensity > 100)		zDensity = 100;
	
	//-- make space for vertices
	RglPoint	**verts = new RglPoint*[zDensity+1];
	RglPoint	**norms = new RglPoint*[zDensity+1];
	for (iz = 0; iz < (zDensity+1); iz++)
	{
		norms[iz] = new RglPoint[xyDensity+1];
		verts[iz] = new RglPoint[xyDensity+1];
		for(ixy = 0; ixy < xyDensity+1; ixy++)
		{
			norms[iz][ixy][0] = 0.0f;
			norms[iz][ixy][1] = 0.0f;
			norms[iz][ixy][2] = 0.0f;
		}
	}
	
	//-- calculate min and max
	radStep	= (D2RAD(thetamax))/( (float)xyDensity );
	rad		= M_PI/2.0;
	for (ixy = 0; ixy < xyDensity+1; ixy++)
	{
		x = sin (rad)*radius;	
		y = cos (rad)*radius;
		verts[0][ixy][0] = verts[zDensity-1][ixy][0] = x;
		verts[0][ixy][1] = verts[zDensity-1][ixy][1] = y;
		verts[0][ixy][2] = zmin;
		verts[zDensity-1][ixy][2] = zmax;
		norms[0][ixy][0] = norms[zDensity-1][ixy][0] = x;
		norms[0][ixy][1] = norms[zDensity-1][ixy][1] = y;
		norms[0][ixy][2] = norms[zDensity-1][ixy][2] = 0.0;
		normalizeVec(norms[0][ixy]);
		normalizeVec(norms[zDensity-1][ixy]);
		rad += radStep;
	}
		
	//-- interpolate tesselation
	float	izf;
	float	zDf = (float)zDensity-1;
	for(iz = 1; iz < zDensity-1; iz++)
	{
		izf = (float)iz;
		for (ixy = 0; ixy < xyDensity+1; ixy++)
		{
			verts[iz][ixy][0] = verts[0][ixy][0]*((zDf-izf)/zDf) + verts[zDensity-1][ixy][0]*(izf/zDf);
			verts[iz][ixy][1] = verts[0][ixy][1]*((zDf-izf)/zDf) + verts[zDensity-1][ixy][1]*(izf/zDf);
			verts[iz][ixy][2] = verts[0][ixy][2]*((zDf-izf)/zDf) + verts[zDensity-1][ixy][2]*(izf/zDf);
			norms[iz][ixy][0] = norms[0][ixy][0];
			norms[iz][ixy][1] = norms[0][ixy][1];
			norms[iz][ixy][2] = norms[0][ixy][2];
		}
	}
	
	//-- Draw cylinder
	float s1, t1, t2, zd, xyd;
	zd	= zDensity-1.0;
	xyd	= xyDensity-1.0;
	
	for(iz = 0; iz < zDensity-1; iz++)
	{
		t1 = (zd-iz)/zd;
		t2 = (zd-iz-1.0)/zd;
		izf = (float)iz;
		glBegin(GL_TRIANGLE_STRIP);
		for (ixy = 0; ixy < xyDensity+1; ixy++)
		{
			s1 = ixy/xyd;
			
			glTexCoord2f_M0(s1, t1);
			glTexCoord2f_M1(s1, t1);
			glNormal3fv(norms[iz][ixy]);
			glVertex3fv(verts[iz][ixy]);
			
			glTexCoord2f_M0(s1, t2);
			glTexCoord2f_M1(s1, t2);
			glNormal3fv(norms[iz+1][ixy]);
			glVertex3fv(verts[iz+1][ixy]);
		}
		glEnd();
	}
	
	//-- Cleanup
	for (iz = 0; iz < (zDensity+1); iz++)
	{
		delete []verts[iz];
		delete []norms[iz];
	}
	delete []verts;
	delete []norms;
	
}

/**
 * RiSphere clone
 */
//--------------------------------------------------------------------
void RglSphere (float radius, float zmin, float zmax, float thetamax, int detail)
{
	int			ixy;
	int			iz;
	int			zDensity;
	int			xyDensity;
	float		x,y,z;
	float		xySize;
	float		rad;
	float		zrad;
	float		zradStep; 
	float		radStep;
	float		zminRad;
	float		zmaxRad;
	
	//-- do some quick error checking
	if (!radius || !thetamax || zmin == zmax )	return;
	if (thetamax >  360.0f)	thetamax = 360.0f;
	if (thetamax < -360.0f)	thetamax = -360.0f;
	if (zmin < -radius)	zmin = -radius;
	if (zmin >  radius)	zmin = radius;
	if (zmax < -radius)	zmax = -radius;
	if (zmax >  radius)	zmax = radius;

	//-- set number of y polygons to half that of detail, & account for partial spheres
	zDensity = abs((int)( ((zmax-zmin)/radius)*(float)detail )/3);
	if (zDensity < 2)	zDensity = 2;
	xyDensity = abs((int)( (D2RAD(thetamax)/(2.0*M_PI))*(float)detail ));
	if (xyDensity < 4)	xyDensity = 4;

	//-- make space for vertices
	RglPoint **verts = new RglPoint*[zDensity+1];
	RglPoint **norms = new RglPoint*[zDensity+1];
	for (iz = 0; iz < (zDensity+1); iz++)
	{
		norms[iz] = new RglPoint[xyDensity+1];
		verts[iz] = new RglPoint[xyDensity+1];
	}
		
	zmaxRad = -acos(zmax/radius);
	zminRad = -acos(zmin/radius);
	
	zradStep = (zmaxRad-zminRad)/(float)(zDensity);
	radStep  = D2RAD(thetamax)/(float)xyDensity;
	
	zrad = zmaxRad;
	
	for (iz = 0; iz < zDensity+1; iz++)
	{
		z = cos(zrad) * radius;
		rad = -M_PI/2.0;
		xySize = sin(zrad) * radius;
		for(ixy = 0; ixy < (xyDensity+1); ixy++)
		{
			x = sin (rad)*xySize;	
			y = cos (rad)*xySize;
			verts[iz][ixy][0] = norms[iz][ixy][0] = x;	
			verts[iz][ixy][1] = norms[iz][ixy][1] = y;
			verts[iz][ixy][2] = norms[iz][ixy][2] = z;
			normalizeVec(norms[iz][ixy]);
			rad += radStep;
		}
		zrad -= zradStep;
	}			

	float s1, t1, t2, zd, xyd;
	zd	= zDensity-1.0;
	xyd	= xyDensity-1.0;
	for (iz = 0; iz < zDensity; iz++)
	{
		t1 = 1.0 - (zd-iz)/zd;
		t2 = 1.0 - (zd-iz-1.0)/zd;
	
		glBegin(GL_QUAD_STRIP);
		if(zmin < zmax) //-- if we want to flip the normals, swap zmin and zmax
			for(ixy = 0; ixy <= xyDensity; ixy++)
			{
				s1 = ixy/xyd;

				glTexCoord2f_M0(s1, t2);
				glTexCoord2f_M1(s1, t2);
				glNormal3fv(norms[iz+1][ixy]);
				glVertex3fv(verts[iz+1][ixy]);
				glTexCoord2f_M0(s1, t1);
				glTexCoord2f_M1(s1, t1);
				glNormal3fv(norms[iz][ixy]);
				glVertex3fv(verts[iz][ixy]);
			}
		else
			for(ixy = 0; ixy <= xyDensity; ixy++)
			{
				s1 = ixy/xyd;
				
				glTexCoord2f_M0(s1, t2);
				glTexCoord2f_M1(s1, t2);
				glNormal3f( -norms[iz+1][ixy][0], 
							-norms[iz+1][ixy][1], 
							-norms[iz+1][ixy][2]);
				glVertex3fv(verts[iz+1][ixy]);
				glTexCoord2f_M0(s1, t1);
				glTexCoord2f_M1(s1, t1);
				glNormal3f( -norms[iz][ixy][0], 
							-norms[iz][ixy][1], 
							-norms[iz][ixy][2]);
				glVertex3fv(verts[iz][ixy]);
			}
		glEnd();
	}
	
	//-- cleanup
	for (iz = 0; iz < (zDensity+1); iz++)
	{
		delete []verts[iz];
		delete []norms[iz];
	}
	delete []verts;
	delete []norms;
}

/**
 * RiSphere clone
 */
//--------------------------------------------------------------------
void RglSphereN (float radius, float zmin, float zmax, float thetamax, int detail, MVertex &nscale)
{
	int			ixy;
	int			iz;
	int			zDensity;
	int			xyDensity;
	float		x,y,z;
	float		xySize;
	float		rad;
	float		zrad;
	float		zradStep; 
	float		radStep;
	float		zminRad;
	float		zmaxRad;
	
	//-- do some quick error checking
	if (!radius || !thetamax || zmin == zmax )	return;
	if (thetamax >  360.0f)	thetamax = 360.0f;
	if (thetamax < -360.0f)	thetamax = -360.0f;
	if (zmin < -radius)	zmin = -radius;
	if (zmin >  radius)	zmin = radius;
	if (zmax < -radius)	zmax = -radius;
	if (zmax >  radius)	zmax = radius;

	//-- set number of y polygons to half that of detail, & account for partial spheres
	zDensity = abs((int)( ((zmax-zmin)/radius)*(float)detail )/3);
	if (zDensity < 2)	zDensity = 2;
	xyDensity = abs((int)( (D2RAD(thetamax)/(2.0*M_PI))*(float)detail ));
	if (xyDensity < 4)	xyDensity = 4;

	//-- make space for vertices
	RglPoint **verts = new RglPoint*[zDensity+1];
	RglPoint **norms = new RglPoint*[zDensity+1];
	for (iz = 0; iz < (zDensity+1); iz++)
	{
		norms[iz] = new RglPoint[xyDensity+1];
		verts[iz] = new RglPoint[xyDensity+1];
	}
		
	zmaxRad = -acos(zmax/radius);
	zminRad = -acos(zmin/radius);
	
	zradStep = (zmaxRad-zminRad)/(float)(zDensity);
	radStep  = D2RAD(thetamax)/(float)xyDensity;
	
	zrad = zmaxRad;
	
	for (iz = 0; iz < zDensity+1; iz++)
	{
		z = cos(zrad) * radius;
		rad = -M_PI/2.0;
		xySize = sin(zrad) * radius;
		for(ixy = 0; ixy < (xyDensity+1); ixy++)
		{
			x = sin (rad)*xySize;	
			y = cos (rad)*xySize;
			verts[iz][ixy][0] = norms[iz][ixy][0] = x;	
			verts[iz][ixy][1] = norms[iz][ixy][1] = y;
			verts[iz][ixy][2] = norms[iz][ixy][2] = z;
			normalizeVec(norms[iz][ixy]);
			rad += radStep;
		}
		zrad -= zradStep;
	}			

	MVertex N1, N2, T;

	float s1, t1, t2, zd, xyd;
	zd	= zDensity-1.0;
	xyd	= xyDensity-1.0;
	for (iz = 0; iz < zDensity; iz++)
	{
		t1 = 1.0 - (zd-iz)/zd;
		t2 = 1.0 - (zd-iz-1.0)/zd;
	
		glBegin(GL_QUAD_STRIP);
		if(zmin < zmax) //-- if we want to flip the normals, swap zmin and zmax
			for(ixy = 0; ixy <= xyDensity; ixy++)
			{
				s1 = ixy/xyd;
				
				T	= norms[iz+1][ixy];
				N1	= nscale.scale(T);
				T	= norms[iz][ixy];
				N2	= nscale.scale(T);
				
				glTexCoord2f_M0(s1, t2);
				glTexCoord2f_M1(s1, t2);
				glNormal3fv( N1.xyz() );
				glVertex3fv(verts[iz+1][ixy]);
				glTexCoord2f_M0(s1, t1);
				glTexCoord2f_M1(s1, t1);
				glNormal3fv( N2.xyz() );
				glVertex3fv(verts[iz][ixy]);
			}
		else
			for(ixy = 0; ixy <= xyDensity; ixy++)
			{
				s1 = ixy/xyd;
				
				glTexCoord2f_M0(s1, t2);
				glTexCoord2f_M1(s1, t2);
				glNormal3f( -norms[iz+1][ixy][0], 
							-norms[iz+1][ixy][1], 
							-norms[iz+1][ixy][2]);
				glVertex3fv(verts[iz+1][ixy]);
				glTexCoord2f_M0(s1, t1);
				glTexCoord2f_M1(s1, t1);
				glNormal3f( -norms[iz][ixy][0], 
							-norms[iz][ixy][1], 
							-norms[iz][ixy][2]);
				glVertex3fv(verts[iz][ixy]);
			}
		glEnd();
	}
	
	//-- cleanup
	for (iz = 0; iz < (zDensity+1); iz++)
	{
		delete []verts[iz];
		delete []norms[iz];
	}
	delete []verts;
	delete []norms;
}

/**
 * RiPatch (bilinear) with some restrictions
 */
//--------------------------------------------------------------------
void RglBilinearPatch (float p[2][2][3], int detail)
{
	float	norm[3];
	int 	i;
	int 	j;
	float	***subdiv;
	double	uInc;
	double	vInc;
	double	u;
	double	v;
	int		uDetail;
	int		vDetail;
	
	if (detail < 1)
		detail = 1;
	
//	uDetail = vDetail = detail;
	//-- figure out a nice subdivision...
	float	tmpA;
	float	tmpB;
	float	tmpC;
	float uDist;
	float vDist;
	tmpA = p[0][0][0]-p[0][1][0];
	tmpB = p[0][0][1]-p[0][1][1];
	tmpC = p[0][0][2]-p[0][1][2];
	uDist = fabs(tmpA) + fabs(tmpB) + fabs(tmpC);
	tmpA = p[1][0][0]-p[1][1][0];
	tmpB = p[1][0][1]-p[1][1][1];
	tmpC = p[1][0][2]-p[1][1][2];
	uDist += fabs(tmpA) + fabs(tmpB) + fabs(tmpC);
	tmpA = p[0][0][0]-p[1][0][0];
	tmpB = p[0][0][1]-p[1][0][1];
	tmpC = p[0][0][2]-p[1][0][2];
	vDist = fabs(tmpA) + fabs(tmpB) + fabs(tmpC);
	tmpA = p[0][1][0]-p[1][1][0];
	tmpB = p[0][1][1]-p[1][1][1];
	tmpC = p[0][1][2]-p[1][1][2];
	vDist += fabs(tmpA) + fabs(tmpB) + fabs(tmpC);
	if(uDist < vDist)
	{
		uDetail = detail;
		vDetail = (int)((float)detail*(uDist/vDist));
	}
	else
	{
		uDetail = (int)((float)detail*(vDist/uDist));
		vDetail = detail;
	}
	//-- sanity checks...
	if(vDetail < 1)
		vDetail = 1;
	if(uDetail < 1)
		uDetail = 1;
	if(vDetail > 100)
		vDetail = 100;
	if(uDetail > 100)
		uDetail = 100;
	
	//-- allocate memory
	subdiv	= new float**[vDetail+1];
	for (i = 0; i < vDetail+1; i++)
	{
		subdiv[i] = new float*[uDetail+1];
		for (j = 0; j < uDetail+1; j++)
			subdiv[i][j] = new float[3];
	}
	
	//-- calculate vertices
	uInc = 1.0/(double)uDetail;
	vInc = 1.0/(double)vDetail;
	v = 0.0;
	for (i = 0; i < vDetail+1; i++)
	{
		u = 0.0;
		for (j = 0; j < uDetail+1; j++)
		{
			subdiv[i][j][0] = (1.0-u)*(1.0-v)*p[0][0][0] + u*(1.0-v)*p[1][0][0] + (1.0-u)*v*p[0][1][0] + u*v*p[1][1][0];
			subdiv[i][j][1] = (1.0-u)*(1.0-v)*p[0][0][1] + u*(1.0-v)*p[1][0][1] + (1.0-u)*v*p[0][1][1] + u*v*p[1][1][1];
			subdiv[i][j][2] = (1.0-u)*(1.0-v)*p[0][0][2] + u*(1.0-v)*p[1][0][2] + (1.0-u)*v*p[0][1][2] + u*v*p[1][1][2];
			// flip Z for gl/RIB compatibility
			subdiv[i][j][2] = -subdiv[i][j][2];
			u += uInc;
		}
		v += vInc;
	}
	
	float s1, s2, t1, t2, ud, vd;
	ud	= uDetail;
	vd	= vDetail;
	//-- draw vertices
	for (i = 0; i < vDetail; i++)
	{	
		//ooops - u,v are backwards...
		s1 = 	 i/vd;
		s2 = (i+1)/vd;
		t1 = 0.0;
		t2 = 0.0;
		
		glBegin(GL_TRIANGLE_STRIP);
		calcNormal	(norm, subdiv[i][1], subdiv[i+1][1], subdiv[i][0]);
		glNormal3fv	(norm);
		
		glTexCoord2f_M0(s2, t1);
		glTexCoord2f_M1(s2, t1);
		glVertex3fv(subdiv[i+1][0]);
		glTexCoord2f_M0(s1, t1);
		glTexCoord2f_M1(s1, t1);
		glVertex3fv(subdiv[i  ][0]);
		for (j = 0; j < uDetail; j++)
		{
			t1 =     j/ud;
			t2 = (j+1)/ud;
		
			calcNormal	(norm, subdiv[i][j+1], subdiv[i+1][j+1], subdiv[i][j]);
			glNormal3fv	(norm);
			glTexCoord2f_M0(s2, t2);
			glTexCoord2f_M1(s2, t2);
			glVertex3fv(subdiv[i+1][j+1]);
			glTexCoord2f_M0(s1, t2);
			glTexCoord2f_M1(s1, t2);
			glVertex3fv(subdiv[i  ][j+1]);
		}
		glEnd();
	}
			
	// Cleanup...
	for (i = 0; i < vDetail+1; i++)
	{
		for (j = 0; j < uDetail+1; j++)
			delete [] subdiv[i][j];
		delete [] subdiv[i];
	}
	delete [] subdiv;
}

/**
 * Absolutely no relation to any RenderMan primitives...
 * order for texArray = top, bottom, sides
 */
//------------------------------------------------
void RglCube(MVertex &c, MVertex &v1, MVertex &v2, float depth, int detail, GLuint *texArray)
{
	float	A0[3] = {	c.x(), 
						c.y(), 
						c.z() };
	float	B0[3] = {	A0[0] + v2.x(),
					 	A0[1] + v2.y(),
					 	A0[2] + v2.z() };
	float	C0[3] = {	B0[0] + v1.x(),
						B0[1] + v1.y(),
						B0[2] + v1.z() };
	float	D0[3] = {	A0[0] + v1.x(),
						A0[1] + v1.y(),
						A0[2] + v1.z() };

	float	p0[2][2][3];
	
	// Get corners A, B, C and D
	p0[0][0][0] = C0[0];	p0[0][0][1] = C0[1];	p0[0][0][2] = -C0[2];
	p0[0][1][0] = B0[0];	p0[0][1][1] = B0[1];	p0[0][1][2] = -B0[2];
	p0[1][1][0] = A0[0];	p0[1][1][1] = A0[1];	p0[1][1][2] = -A0[2];
	p0[1][0][0] = D0[0];	p0[1][0][1] = D0[1];	p0[1][0][2] = -D0[2];

	GLuint curTex = 0;
	if(texArray) 
	{ curTex = texArray[0]; glBindTexture(GL_TEXTURE_2D, curTex);	}
	RglBilinearPatch(p0, detail);
	//-- if plane has depth, draw it...
	if (depth != 0.0f)
	{
		MVertex	v1(D0[0], D0[1], D0[2]);
		MVertex v2(A0[0], A0[1], A0[2]);
		MVertex v3(B0[0], B0[1], B0[2]);
		MVertex normal;
		normal.CreateNormal(v1, v2, v3);
		normal.setX(normal.x()*depth);
		normal.setY(normal.y()*depth);
		normal.setZ(normal.z()*depth);

		float	A1[3];
		float	B1[3];
		float	C1[3];
		float	D1[3];
		A1[0] = A0[0]-normal.x();
		A1[1] = A0[1]-normal.y();
		A1[2] = A0[2]-normal.z();
		B1[0] = B0[0]-normal.x();
		B1[1] = B0[1]-normal.y();
		B1[2] = B0[2]-normal.z();
		C1[0] = C0[0]-normal.x();
		C1[1] = C0[1]-normal.y();
		C1[2] = C0[2]-normal.z();
		D1[0] = D0[0]-normal.x();
		D1[1] = D0[1]-normal.y();
		D1[2] = D0[2]-normal.z();

		p0[0][0][0] = C1[0];	p0[0][0][1] = C1[1];	p0[0][0][2] = -C1[2];
		p0[0][1][0] = D1[0];	p0[0][1][1] = D1[1];	p0[0][1][2] = -D1[2];
		p0[1][1][0] = A1[0];	p0[1][1][1] = A1[1];	p0[1][1][2] = -A1[2];
		p0[1][0][0] = B1[0];	p0[1][0][1] = B1[1];	p0[1][0][2] = -B1[2];
		float	p1[2][2][3];
		float	p2[2][2][3];
		float	p3[2][2][3];
		float	p4[2][2][3];
		p1[0][0][0] = B0[0];	p1[0][0][1] = B0[1];	p1[0][0][2] = -B0[2];
		p1[0][1][0] = B1[0];	p1[0][1][1] = B1[1];	p1[0][1][2] = -B1[2];
		p1[1][1][0] = A1[0];	p1[1][1][1] = A1[1];	p1[1][1][2] = -A1[2];
		p1[1][0][0] = A0[0];	p1[1][0][1] = A0[1];	p1[1][0][2] = -A0[2];
		p2[0][0][0] = C0[0];	p2[0][0][1] = C0[1];	p2[0][0][2] = -C0[2];
		p2[0][1][0] = C1[0];	p2[0][1][1] = C1[1];	p2[0][1][2] = -C1[2];
		p2[1][1][0] = B1[0];	p2[1][1][1] = B1[1];	p2[1][1][2] = -B1[2];
		p2[1][0][0] = B0[0];	p2[1][0][1] = B0[1];	p2[1][0][2] = -B0[2];
		p3[0][0][0] = D0[0];	p3[0][0][1] = D0[1];	p3[0][0][2] = -D0[2];
		p3[0][1][0] = D1[0];	p3[0][1][1] = D1[1];	p3[0][1][2] = -D1[2];
		p3[1][1][0] = C1[0];	p3[1][1][1] = C1[1];	p3[1][1][2] = -C1[2];
		p3[1][0][0] = C0[0];	p3[1][0][1] = C0[1];	p3[1][0][2] = -C0[2];
		p4[0][0][0] = A0[0];	p4[0][0][1] = A0[1];	p4[0][0][2] = -A0[2];
		p4[0][1][0] = A1[0];	p4[0][1][1] = A1[1];	p4[0][1][2] = -A1[2];
		p4[1][1][0] = D1[0];	p4[1][1][1] = D1[1];	p4[1][1][2] = -D1[2];
		p4[1][0][0] = D0[0];	p4[1][0][1] = D0[1];	p4[1][0][2] = -D0[2];

		if(texArray)
		{
			//if(texArray[1] != curTex)
			{ curTex = texArray[1];	glBindTexture(GL_TEXTURE_2D, curTex);	}
			RglBilinearPatch(p0, detail);
			//if(texArray[2] != curTex)
			{ curTex = texArray[2];	glBindTexture(GL_TEXTURE_2D, curTex);	}
			RglBilinearPatch(p1, detail);
			//if(texArray[3] != curTex)
			{ curTex = texArray[3];	glBindTexture(GL_TEXTURE_2D, curTex);	}
			RglBilinearPatch(p2, detail);
			//if(texArray[4] != curTex)
			{ curTex = texArray[4];	glBindTexture(GL_TEXTURE_2D, curTex);	}
			RglBilinearPatch(p3, detail);
			//if(texArray[5] != curTex)
			{ curTex = texArray[5];	glBindTexture(GL_TEXTURE_2D, curTex);	}
			RglBilinearPatch(p4, detail);
		}
		else
		{
			RglBilinearPatch(p0, detail);
			RglBilinearPatch(p1, detail);
			RglBilinearPatch(p2, detail);
			RglBilinearPatch(p3, detail);
			RglBilinearPatch(p4, detail);
		}
	}
}
	
