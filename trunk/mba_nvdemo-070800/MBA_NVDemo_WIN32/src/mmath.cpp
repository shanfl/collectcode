/**
 * mmath.cpp
 * Copyright 1999 by Mark B. Allan
 * 
 * WARNING!!: Don't rely on these routines being correct! My math
 * isn't very good...
 */
#include "mmath.h"

#include <stdlib.h>

//==============================================================================
//                                                                    MVertex
//==============================================================================
MVertex::MVertex (float inX, float inY, float inZ, float inW)
{
	vert[0] = inX;
	vert[1] = inY;
	vert[2] = inZ;
	vert[3] = inW;
}

MVertex::MVertex (float *in)
{
	vert[0] = in[0];
	vert[1] = in[1];
	vert[2] = in[2];
}

MVertex::MVertex (const MVertex &in)
{
	vert[0] = in.vert[0];
	vert[1] = in.vert[1];
	vert[2] = in.vert[2];
	vert[3] = in.vert[3];
}

//----------------------------
MVertex	MVertex::operator = (const MVertex &in)
{
	vert[0] = in.vert[0];
	vert[1] = in.vert[1];
	vert[2] = in.vert[2];
	vert[3] = in.vert[3];
	return *this;
}

//----------------------------
MVertex	MVertex::operator + (const MVertex &in)
{
	MVertex	ret;
	ret.set(vert[0]+in.vert[0], vert[1]+in.vert[1], vert[2]+in.vert[2]);
	return ret;
}

//----------------------------
MVertex	MVertex::operator - (const MVertex &in)
{
	MVertex	ret;
	ret.set(vert[0]-in.vert[0], vert[1]-in.vert[1], vert[2]-in.vert[2]);
	return ret;
}

//----------------------------
MVertex	MVertex::operator * (const MVertex &in)
{
	MVertex	ret;
	ret.setX(vert[0]*in.vert[0] + vert[0]*in.vert[1] + vert[0]*in.vert[2]);
	ret.setY(vert[1]*in.vert[0] + vert[1]*in.vert[1] + vert[1]*in.vert[2]);
	ret.setZ(vert[2]*in.vert[0] + vert[2]*in.vert[1] + vert[2]*in.vert[2]);
	return ret;
}

//-----------------------------------------------------
void	MVertex::divByW()
{
	vert[0] /= vert[3];
	vert[1] /= vert[3];
	vert[2] /= vert[3];
	vert[3]  = 1.0;
}

//-----------------------------------------------------
float	MVertex::length()
{
	float	len;
	len = sqrt( (vert[0]*vert[0]) + (vert[1]*vert[1]) + (vert[2]*vert[2]) );
	return len;
}

//-----------------------------------------------------
MVertex&	MVertex::normalize()
{
	float	len;
	
	len = length();
		
	if (len == 0.0)
		len = 1.0;
		
	vert[0] /= len;
	vert[1] /= len;
	vert[2] /= len;
	
	return *this;
}

//-----------------------------------------------------
MVertex&	MVertex::CreateNormal(MVertex &v1, MVertex &v2, MVertex &v3)
{
	MVertex	u1;
	MVertex	u2;
	
	u1.setX ( v1.X() - v2.X() );
	u1.setY ( v1.Y() - v2.Y() );
	u1.setZ ( v1.Z() - v2.Z() );
			
	u2.setX ( v2.X() - v3.X() );
	u2.setY ( v2.Y() - v3.Y() );
	u2.setZ ( v2.Z() - v3.Z() );
			
	vert[0] = u1.Y()*u2.Z() - u1.Z()*u2.Y();		
	vert[1] = u1.Z()*u2.X() - u1.X()*u2.Z();		
	vert[2] = u1.X()*u2.Y() - u1.Y()*u2.X();	
		
	normalize();
	return *this;
}

//==================================================================================================
//                                                                    MMatrix
//==================================================================================================
MMatrix::MMatrix()
{
	matx[ 0] = 1.0;	matx[ 1] = 0.0;	matx[ 2] = 0.0;	matx[ 3] = 0.0;	
	matx[ 4] = 0.0;	matx[ 5] = 1.0;	matx[ 6] = 0.0;	matx[ 7] = 0.0;	
	matx[ 8] = 0.0;	matx[ 9] = 0.0;	matx[10] = 1.0;	matx[11] = 0.0;	
	matx[12] = 0.0;	matx[13] = 0.0;	matx[14] = 0.0;	matx[15] = 1.0;
}

MMatrix::MMatrix(const MMatrix& in)
{
	matx[ 0] = in.matx[ 0];
	matx[ 1] = in.matx[ 1];
	matx[ 2] = in.matx[ 2];
	matx[ 3] = in.matx[ 3];
	matx[ 4] = in.matx[ 4];
	matx[ 5] = in.matx[ 5];
	matx[ 6] = in.matx[ 6];
	matx[ 7] = in.matx[ 7];
	matx[ 8] = in.matx[ 8];
	matx[ 9] = in.matx[ 9];
	matx[10] = in.matx[10];
	matx[11] = in.matx[11];
	matx[12] = in.matx[12];
	matx[13] = in.matx[13];
	matx[14] = in.matx[14];
	matx[15] = in.matx[15];
}

MMatrix::~MMatrix()
{
}

//--------------------------------------------------------------------
MMatrix	MMatrix::operator =(const MMatrix& in)
{
	matx[ 0] = in.matx[ 0];
	matx[ 1] = in.matx[ 1];
	matx[ 2] = in.matx[ 2];
	matx[ 3] = in.matx[ 3];
	matx[ 4] = in.matx[ 4];
	matx[ 5] = in.matx[ 5];
	matx[ 6] = in.matx[ 6];
	matx[ 7] = in.matx[ 7];
	matx[ 8] = in.matx[ 8];
	matx[ 9] = in.matx[ 9];
	matx[10] = in.matx[10];
	matx[11] = in.matx[11];
	matx[12] = in.matx[12];
	matx[13] = in.matx[13];
	matx[14] = in.matx[14];
	matx[15] = in.matx[15];
	return *this;
}

//--------------------------------------------------------------------
MMatrix	MMatrix::operator *(const MMatrix& in)
{
	MMatrix	ret;
		
	ret.matx [0] = matx [0]*in.matx [0] + matx [1]*in.matx [4] + matx [2]*in.matx [8] + matx [3]*in.matx[12];
	ret.matx [1] = matx [0]*in.matx [1] + matx [1]*in.matx [5] + matx [2]*in.matx [9] + matx [3]*in.matx[13];
	ret.matx [2] = matx [0]*in.matx [2] + matx [1]*in.matx [6] + matx [2]*in.matx[10] + matx [3]*in.matx[14];
	ret.matx [3] = matx [0]*in.matx [3] + matx [1]*in.matx [7] + matx [2]*in.matx[11] + matx [3]*in.matx[15];

	ret.matx[ 4] = matx[ 4]*in.matx[ 0] + matx[ 5]*in.matx[ 4] + matx[ 6]*in.matx[ 8] + matx[ 7]*in.matx[12];
	ret.matx[ 5] = matx[ 4]*in.matx[ 1] + matx[ 5]*in.matx[ 5] + matx[ 6]*in.matx[ 9] + matx[ 7]*in.matx[13];
	ret.matx[ 6] = matx[ 4]*in.matx[ 2] + matx[ 5]*in.matx[ 6] + matx[ 6]*in.matx[10] + matx[ 7]*in.matx[14];
	ret.matx[ 7] = matx[ 4]*in.matx[ 3] + matx[ 5]*in.matx[ 7] + matx[ 6]*in.matx[11] + matx[ 7]*in.matx[15];

	ret.matx[ 8] = matx[ 8]*in.matx[ 0] + matx[ 9]*in.matx[ 4] + matx[10]*in.matx[ 8] + matx[11]*in.matx[12];
	ret.matx[ 9] = matx[ 8]*in.matx[ 1] + matx[ 9]*in.matx[ 5] + matx[10]*in.matx[ 9] + matx[11]*in.matx[13];
	ret.matx[10] = matx[ 8]*in.matx[ 2] + matx[ 9]*in.matx[ 6] + matx[10]*in.matx[10] + matx[11]*in.matx[14];
	ret.matx[11] = matx[ 8]*in.matx[ 3] + matx[ 9]*in.matx[ 7] + matx[10]*in.matx[11] + matx[11]*in.matx[15];

	ret.matx[12] = matx[12]*in.matx[ 0] + matx[13]*in.matx[ 4] + matx[14]*in.matx[ 8] + matx[15]*in.matx[12];
	ret.matx[13] = matx[12]*in.matx[ 1] + matx[13]*in.matx[ 5] + matx[14]*in.matx[ 9] + matx[15]*in.matx[13];
	ret.matx[14] = matx[12]*in.matx[ 2] + matx[13]*in.matx[ 6] + matx[14]*in.matx[10] + matx[15]*in.matx[14];
	ret.matx[15] = matx[12]*in.matx[ 3] + matx[13]*in.matx[ 7] + matx[14]*in.matx[11] + matx[15]*in.matx[15];
	
	return ret;
}

//--------------------------------------------------------------------
MVertex	MMatrix::operator *(const MVertex& in)
{
	MVertex	returnVert;

	returnVert.setX( matx[ 0]*in.vert[0] + matx[ 4]*in.vert[1] + matx[ 8]*in.vert[2] + matx[12] );
	returnVert.setY( matx[ 1]*in.vert[0] + matx[ 5]*in.vert[1] + matx[ 9]*in.vert[2] + matx[13] );
	returnVert.setZ( matx[ 2]*in.vert[0] + matx[ 6]*in.vert[1] + matx[10]*in.vert[2] + matx[14] );
																 	
//	returnVert.setX( matx[ 0]*in.X() + matx[ 4]*in.Y() + matx[ 8]*in.Z() + matx[12] );
//	returnVert.setY( matx[ 1]*in.X() + matx[ 5]*in.Y() + matx[ 9]*in.Z() + matx[13] );
//	returnVert.setZ( matx[ 2]*in.X() + matx[ 6]*in.Y() + matx[10]*in.Z() + matx[14] );
//																 	
	return returnVert;											 
}
																 
//--------------------------------------------------------------------
MMatrix&	MMatrix::operator *=(const MMatrix& in)
{
	float	val[16];
	val[ 0] = matx[ 0]*in.matx[ 0] + matx[ 1]*in.matx[ 4] + matx[ 2]*in.matx[ 8] + matx[ 3]*in.matx[12];
	val[ 1] = matx[ 0]*in.matx[ 1] + matx[ 1]*in.matx[ 5] + matx[ 2]*in.matx[ 9] + matx[ 3]*in.matx[13];
	val[ 2] = matx[ 0]*in.matx[ 2] + matx[ 1]*in.matx[ 6] + matx[ 2]*in.matx[10] + matx[ 3]*in.matx[14];
	val[ 3] = matx[ 0]*in.matx[ 3] + matx[ 1]*in.matx[ 7] + matx[ 2]*in.matx[11] + matx[ 3]*in.matx[15];
	val[ 4] = matx[ 4]*in.matx[ 0] + matx[ 5]*in.matx[ 4] + matx[ 6]*in.matx[ 8] + matx[ 7]*in.matx[12];
	val[ 5] = matx[ 4]*in.matx[ 1] + matx[ 5]*in.matx[ 5] + matx[ 6]*in.matx[ 9] + matx[ 7]*in.matx[13];
	val[ 6] = matx[ 4]*in.matx[ 2] + matx[ 5]*in.matx[ 6] + matx[ 6]*in.matx[10] + matx[ 7]*in.matx[14];
	val[ 7] = matx[ 4]*in.matx[ 3] + matx[ 5]*in.matx[ 7] + matx[ 6]*in.matx[11] + matx[ 7]*in.matx[15];
	val[ 8] = matx[ 8]*in.matx[ 0] + matx[ 9]*in.matx[ 4] + matx[10]*in.matx[ 8] + matx[11]*in.matx[12];
	val[ 9] = matx[ 8]*in.matx[ 1] + matx[ 9]*in.matx[ 5] + matx[10]*in.matx[ 9] + matx[11]*in.matx[13];
	val[10] = matx[ 8]*in.matx[ 2] + matx[ 9]*in.matx[ 6] + matx[10]*in.matx[10] + matx[11]*in.matx[14];
	val[11] = matx[ 8]*in.matx[ 3] + matx[ 9]*in.matx[ 7] + matx[10]*in.matx[11] + matx[11]*in.matx[15];
	val[12] = matx[12]*in.matx[ 0] + matx[13]*in.matx[ 4] + matx[14]*in.matx[ 8] + matx[15]*in.matx[12];
	val[13] = matx[12]*in.matx[ 1] + matx[13]*in.matx[ 5] + matx[14]*in.matx[ 9] + matx[15]*in.matx[13];
	val[14] = matx[12]*in.matx[ 2] + matx[13]*in.matx[ 6] + matx[14]*in.matx[10] + matx[15]*in.matx[14];
	val[15] = matx[12]*in.matx[ 3] + matx[13]*in.matx[ 7] + matx[14]*in.matx[11] + matx[15]*in.matx[15];
	matx[ 0] = val[ 0];
	matx[ 1] = val[ 1];
	matx[ 2] = val[ 2];
	matx[ 3] = val[ 3];
	matx[ 4] = val[ 4];
	matx[ 5] = val[ 5];
	matx[ 6] = val[ 6];
	matx[ 7] = val[ 7];
	matx[ 8] = val[ 8];
	matx[ 9] = val[ 9];
	matx[10] = val[10];
	matx[11] = val[11];
	matx[12] = val[12];
	matx[13] = val[13];
	matx[14] = val[14];
	matx[15] = val[15];
	return *this;
}

//--------------------------------------------------------------------
MMatrix&	MMatrix::operator *=(const float *in)
{
	float	val[16];
	val[ 0] = matx[ 0]*in[ 0] + matx[ 1]*in[ 4] + matx[ 2]*in[ 8] + matx[ 3]*in[12];
	val[ 1] = matx[ 0]*in[ 1] + matx[ 1]*in[ 5] + matx[ 2]*in[ 9] + matx[ 3]*in[13];
	val[ 2] = matx[ 0]*in[ 2] + matx[ 1]*in[ 6] + matx[ 2]*in[10] + matx[ 3]*in[14];
	val[ 3] = matx[ 0]*in[ 3] + matx[ 1]*in[ 7] + matx[ 2]*in[11] + matx[ 3]*in[15];
	val[ 4] = matx[ 4]*in[ 0] + matx[ 5]*in[ 4] + matx[ 6]*in[ 8] + matx[ 7]*in[12];
	val[ 5] = matx[ 4]*in[ 1] + matx[ 5]*in[ 5] + matx[ 6]*in[ 9] + matx[ 7]*in[13];
	val[ 6] = matx[ 4]*in[ 2] + matx[ 5]*in[ 6] + matx[ 6]*in[10] + matx[ 7]*in[14];
	val[ 7] = matx[ 4]*in[ 3] + matx[ 5]*in[ 7] + matx[ 6]*in[11] + matx[ 7]*in[15];
	val[ 8] = matx[ 8]*in[ 0] + matx[ 9]*in[ 4] + matx[10]*in[ 8] + matx[11]*in[12];
	val[ 9] = matx[ 8]*in[ 1] + matx[ 9]*in[ 5] + matx[10]*in[ 9] + matx[11]*in[13];
	val[10] = matx[ 8]*in[ 2] + matx[ 9]*in[ 6] + matx[10]*in[10] + matx[11]*in[14];
	val[11] = matx[ 8]*in[ 3] + matx[ 9]*in[ 7] + matx[10]*in[11] + matx[11]*in[15];
	val[12] = matx[12]*in[ 0] + matx[13]*in[ 4] + matx[14]*in[ 8] + matx[15]*in[12];
	val[13] = matx[12]*in[ 1] + matx[13]*in[ 5] + matx[14]*in[ 9] + matx[15]*in[13];
	val[14] = matx[12]*in[ 2] + matx[13]*in[ 6] + matx[14]*in[10] + matx[15]*in[14];
	val[15] = matx[12]*in[ 3] + matx[13]*in[ 7] + matx[14]*in[11] + matx[15]*in[15];
	matx[ 0] = val[ 0];
	matx[ 1] = val[ 1];
	matx[ 2] = val[ 2];
	matx[ 3] = val[ 3];
	matx[ 4] = val[ 4];
	matx[ 5] = val[ 5];
	matx[ 6] = val[ 6];
	matx[ 7] = val[ 7];
	matx[ 8] = val[ 8];
	matx[ 9] = val[ 9];
	matx[10] = val[10];
	matx[11] = val[11];
	matx[12] = val[12];
	matx[13] = val[13];
	matx[14] = val[14];
	matx[15] = val[15];
	return *this;
}

////--------------------------------------------------------------------
//float**	MMatrix::mat4x4()
//{
//	matx4x4[0][0] = matx[ 0];	matx4x4[0][1] = matx[ 1];	matx4x4[0][2] = matx[ 2];	matx4x4[0][3] = matx[ 3];
//	matx4x4[1][0] = matx[ 4];	matx4x4[1][1] = matx[ 5];	matx4x4[1][2] = matx[ 6];	matx4x4[1][3] = matx[ 7];
//	matx4x4[2][0] = matx[ 8];	matx4x4[2][1] = matx[ 9];	matx4x4[2][2] = matx[10];	matx4x4[2][3] = matx[11];
//	matx4x4[3][0] = matx[12];	matx4x4[3][1] = matx[13];	matx4x4[3][2] = matx[14];	matx4x4[3][3] = matx[15];
////
////	fprintf(stderr, "%f %f %f %f\n", matx[ 0], matx[ 1], matx[ 2], matx[ 3]);
////	fprintf(stderr, "%f %f %f %f\n", matx[ 4], matx[ 5], matx[ 6], matx[ 7]);
////	fprintf(stderr, "%f %f %f %f\n", matx[ 8], matx[ 9], matx[10], matx[11]);
////	fprintf(stderr, "%f %f %f %f\n\n", matx[12], matx[13], matx[14], matx[15]);
//	return (float**)matx4x4;
//}

//--------------------------------------------------------------------
void	MMatrix::setIdentity()
{
	matx[ 0] = 1.0;	matx[ 1] = 0.0;	matx[ 2] = 0.0;	matx[ 3] = 0.0;	
	matx[ 4] = 0.0;	matx[ 5] = 1.0;	matx[ 6] = 0.0;	matx[ 7] = 0.0;	
	matx[ 8] = 0.0;	matx[ 9] = 0.0;	matx[10] = 1.0;	matx[11] = 0.0;	
	matx[12] = 0.0;	matx[13] = 0.0;	matx[14] = 0.0;	matx[15] = 1.0;
}

//--------------------------------------------------------------------
void	MMatrix::translate(float inX, float inY, float inZ)
{
	matx[12] += inX;
	matx[13] += inY;
	matx[14] += inZ;
}
//--------------------------------------------------------------------
void	MMatrix::setTranslate(float inX, float inY, float inZ)
{
	matx[12] = inX;
	matx[13] = inY;
	matx[14] = inZ;
}

//--------------------------------------------------------------------
void	MMatrix::setScale(float inX, float inY, float inZ)
{
	matx[ 0] = inX;
	matx[ 5] = inY;
	matx[10] = inZ;
}

//--------------------------------------------------------------------
void	MMatrix::rotateX(float inD)
{
	MMatrix	rotate;
	float	inDrad = 2.0*M_PI*inD/360.0;
	rotate.matx[ 5] =  cos(inDrad);
	rotate.matx[ 6] =  sin(inDrad);
	rotate.matx[ 9] = -rotate.matx[ 6];
	rotate.matx[10] =  rotate.matx[ 5];
	*this *= rotate;
}
	
//--------------------------------------------------------------------
void	MMatrix::rotateY(float inD)
{
	MMatrix	rotate;
	float	inDrad = 2.0*M_PI*inD/360.0;
	rotate.matx[ 0] =  cos(inDrad);
	rotate.matx[ 2] = -sin(inDrad);
	rotate.matx[ 8] = -rotate.matx[ 2];
	rotate.matx[10] =  rotate.matx[ 0];
	*this *= rotate;
}

//--------------------------------------------------------------------
void	MMatrix::rotateZ(float inD)
{
	MMatrix	rotate;
	float	inDrad = 2.0*M_PI*inD/360.0;
	rotate.matx[ 0] =  cos(inDrad);
	rotate.matx[ 1] =  sin(inDrad);
	rotate.matx[ 4] = -rotate.matx[ 1];
	rotate.matx[ 5] =  rotate.matx[ 0];
	*this *= rotate;
}

//--------------------------------------------------------------------
void	MMatrix::pointAt(const MVertex &dir)
{	
	MVertex R;
	MVertex U;
	MVertex RxU;
	
	R = dir;
	if( !(fabs(R.x())+fabs(R.y())+fabs(R.z())) )
		R.set(0.0, 1.0, 0.0);
	R.normalize();
	U = R;
	if(R.x())
		if(R.x() > 0.99999999)
			U.set(0.0, 1.0, 0.0);
		else if(R.x() < -0.99999999)
			U.set(0.0,-1.0, 0.0);
		else
			U.setX(R.x()-1.0/R.x());
	else
		U.set(1.0, 0.0, 0.0);
	U.normalize();
	RxU = U.cross(R);
	RxU.normalize();
	U = R.cross(RxU);
	
	setIdentity();
	matx[ 0] = RxU.x();	matx[ 1] = RxU.y();	matx[ 2] = RxU.z();
	matx[ 4] =   U.x();	matx[ 5] =   U.y();	matx[ 6] =   U.z();
	matx[ 8] =   R.x();	matx[ 9] =   R.y();	matx[10] =   R.z();
	
}

//--------------------------------------------------------------------
void	MMatrix::pointAt(const MVertex &dir, const MVertex &upDir)
{	
	MVertex R;
	MVertex U;
	MVertex RxU;
	
	MVertex	up(upDir);
	float upX = up.x();
	float upY = up.y();
	float upZ = up.z();
	
	R = dir;
	if( !(fabs(R.x())+fabs(R.y())+fabs(R.z())) )
		R.set(0.0, 1.0, 0.0);
	R.normalize();
	if( (fabs(upX)+fabs(upY)+fabs(upZ)) < 0.001 )
	{	upX = 0.0; upY = 1.0; upZ = 0.0;	}
	U.set(upX, upY, upZ);
	U.normalize();
	RxU = U.cross(R);
	RxU.normalize();
	U = R.cross(RxU);
	
	setIdentity();
	matx[ 0] = RxU.x();	matx[ 1] = RxU.y();	matx[ 2] = RxU.z();
	matx[ 4] =   U.x();	matx[ 5] =   U.y();	matx[ 6] =   U.z();
	matx[ 8] =   R.x();	matx[ 9] =   R.y();	matx[10] =   R.z();
	
}


////--------------------------------------------------------------------
//void	MMatrix::RIBConcatTransform()
//{
//	mat4x4();
//	RiConcatTransform(matx4x4);
//}
//
