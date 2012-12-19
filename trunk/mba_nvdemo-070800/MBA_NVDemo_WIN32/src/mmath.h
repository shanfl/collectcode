/**
 * mmath.h
 * Copyright 1999 by Mark B. Allan
 * 
 * WARNING!!: Don't rely on these routines being correct! My math
 * isn't very good...
 */
 
#ifndef mmath_h
#define mmath_h

#include "stdio.h"
#include "math.h"
#include "winstuff.h"

#define D2RAD(a) (2.0*M_PI*a/360.0)

/**
 * vertex class
 */
//======================================================================
class	MVertex
{
public:
	MVertex (float inX = 0.0f, float inY = 0.0f, float inZ = 0.0f, float inW = 1.0f);
	MVertex (float *in);
	MVertex (const MVertex &in);
	MVertex		operator	= (const MVertex &in);
	MVertex		operator	+ (const MVertex &in);
	MVertex		operator	- (const MVertex &in);
	MVertex		operator    * (const MVertex &in);
	
	inline MVertex	operator * (const float in)		{	return MVertex( vert[0]*in, vert[1]*in, vert[2]*in );	}	
	inline MVertex	operator / (const float in)		{	return MVertex( vert[0]/in, vert[1]/in, vert[2]/in );	}
	inline MVertex	operator ^ (const MVertex& in)	{	return MVertex(	vert[1]*in.vert[2] - vert[2]*in.vert[1],
																		vert[2]*in.vert[0] - vert[0]*in.vert[2],
																		vert[0]*in.vert[1] - vert[1]*in.vert[0]);}
													
	
	inline	float	X()		{	return vert[0];	}
	inline	float	Y()		{	return vert[1];	}
	inline	float	Z()		{	return vert[2];	}
	inline	float	*XYZ()	{	return vert;	}
//
	inline	float	x()		{	return vert[0];	}
	inline	float	y()		{	return vert[1];	}
	inline	float	z()		{	return vert[2];	}
	inline	float	*xyz()	{	return vert;	}

	inline	float		dot(const MVertex &in) 
	{ return vert[0]*in.vert[0] + vert[1]*in.vert[1] + vert[2]*in.vert[2]; }

	inline	MVertex		cross(const MVertex &v2)
	{
		return MVertex( vert[1]*v2.vert[2] - vert[2]*v2.vert[1],
						vert[2]*v2.vert[0] - vert[0]*v2.vert[2],
						vert[0]*v2.vert[1] - vert[1]*v2.vert[0]);
	}
	
	MVertex&	normalize();
	float		length();
	inline float Magnitude() { return length(); } /* depreciated. Use length() */
	MVertex&	CreateNormal(MVertex &v1, MVertex &v2, MVertex &v3);
	
	float		manhattan() { return (fabs(vert[0]) + fabs(vert[1]) + fabs(vert[2])); }
		
	void		divByW();	

	inline float	addX(float inX)	{	vert[0] += inX; return vert[0]; }
	inline float	addY(float inY)	{	vert[1] += inY; return vert[1]; }
	inline float	addZ(float inZ) {	vert[2] += inZ; return vert[2]; }
	inline float	setX(float inX)	{   vert[0] = inX;  return vert[0]; }
	inline float	setY(float inY)	{   vert[1] = inY;  return vert[1]; }
	inline float	setZ(float inZ)	{   vert[2] = inZ;  return vert[2]; }
	inline MVertex& add (MVertex &in)	{ vert[0]+=in.vert[0]; vert[1]+=in.vert[1]; vert[2]+=in.vert[2]; return *this; }
	inline MVertex&	set (const MVertex &in)	{ vert[0]=in.vert[0];	vert[1]=in.vert[1]; vert[2]=in.vert[2]; return *this;	}
	inline MVertex&	set (float *in)		{ vert[0] = in[0];	vert[1]=in[1];	vert[2]=in[2];	return *this;	}
	inline MVertex&	set (float inX, float inY, float inZ)
										{ vert[0] = inX;	vert[1]=inY;	vert[2]=inZ;	return *this;	}
	inline MVertex&	Invert()			{ vert[0] = -vert[0]; vert[1] = -vert[1]; vert[2] = -vert[2]; return *this;	}

	inline MVertex& scale(MVertex &in)
	{
		in.vert[0] = vert[0] * in.vert[0];
		in.vert[1] = vert[1] * in.vert[1];
		in.vert[2] = vert[2] * in.vert[2];
		return in;
	}
	
private:
	float	vert[4];

friend class MMatrix;
};

/**
 * matrix class
 */
//======================================================================
class MMatrix
{
public:
	MMatrix();
	MMatrix(const MMatrix& in);
	~MMatrix();
	
	MMatrix    operator =(const MMatrix& in);
	MMatrix    operator *(const MMatrix& in);
	MVertex    operator *(const MVertex& in);
	MMatrix&    operator *=(const float *in);
	MMatrix&    operator *=(const MMatrix& in);

	void	setIdentity();
	inline	void identity() { setIdentity(); }
	void	translate(float inX, float inY, float inZ);
	void	setTranslate(float inX, float inY, float inZ);
	void	setScale(float inX, float inY, float inZ);
	void	rotateX(float inD);
	void	rotateY(float inD);
	void	rotateZ(float inD);

	void	pointAt(const MVertex &dir);
	void	pointAt(const MVertex &dir, const MVertex &up);
	
//	float**	mat4x4();
	inline float*	mat()
	{
		return (float *)matx;
	}
	inline float**	mat4x4()
	{
		matx4x4[0][0] = matx[ 0];	matx4x4[0][1] = matx[ 1];	matx4x4[0][2] = matx[ 2];	matx4x4[0][3] = matx[ 3];
		matx4x4[1][0] = matx[ 4];	matx4x4[1][1] = matx[ 5];	matx4x4[1][2] = matx[ 6];	matx4x4[1][3] = matx[ 7];
		matx4x4[2][0] = matx[ 8];	matx4x4[2][1] = matx[ 9];	matx4x4[2][2] = matx[10];	matx4x4[2][3] = matx[11];
		matx4x4[3][0] = matx[12];	matx4x4[3][1] = matx[13];	matx4x4[3][2] = matx[14];	matx4x4[3][3] = matx[15];
		return (float**)matx4x4;
	}

//	void	RIBConcatTransform();

//protected:
public:
	float	matx[16];
	float	matx4x4[4][4];
	
friend class MVertex;
};

#endif
