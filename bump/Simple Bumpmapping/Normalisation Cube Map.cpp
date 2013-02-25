//////////////////////////////////////////////////////////////////////////////////////////
//	Normalisation Cube Map.cpp
//	Generate normalisation cube map
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	
#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include "Maths/Maths.h"
#include "Normalisation Cube Map.h"

bool GenerateNormalisationCubeMap()
{
	unsigned char * data=new unsigned char[32*32*3];
	if(!data)
	{
		printf("Unable to allocate memory for texture data for cube map\n");
		return false;
	}

	//some useful variables
	int size=32;
	float offset=0.0f;
	float halfSize=16.0f;
	VECTOR3D tempVector;
	unsigned char * bytePtr;

	//positive x
	bytePtr=data;

	for(int j=0; j<size; j++)
	{
		for(int i=0; i<size; i++)
		{
			tempVector.SetX(halfSize);
			tempVector.SetY(-(j+offset-halfSize));
			tempVector.SetZ(-(i+offset-halfSize));

			tempVector.Normalize();
			tempVector.PackTo01();

			bytePtr[0]=(unsigned char)(tempVector.GetX()*255);
			bytePtr[1]=(unsigned char)(tempVector.GetY()*255);
			bytePtr[2]=(unsigned char)(tempVector.GetZ()*255);

			bytePtr+=3;
		}
	}
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
					0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	//negative x
	bytePtr=data;

	for(int j=0; j<size; j++)
	{
		for(int i=0; i<size; i++)
		{
			tempVector.SetX(-halfSize);
			tempVector.SetY(-(j+offset-halfSize));
			tempVector.SetZ((i+offset-halfSize));

			tempVector.Normalize();
			tempVector.PackTo01();

			bytePtr[0]=(unsigned char)(tempVector.GetX()*255);
			bytePtr[1]=(unsigned char)(tempVector.GetY()*255);
			bytePtr[2]=(unsigned char)(tempVector.GetZ()*255);

			bytePtr+=3;
		}
	}
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
					0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	//positive y
	bytePtr=data;

	for(int j=0; j<size; j++)
	{
		for(int i=0; i<size; i++)
		{
			tempVector.SetX(i+offset-halfSize);
			tempVector.SetY(halfSize);
			tempVector.SetZ((j+offset-halfSize));

			tempVector.Normalize();
			tempVector.PackTo01();

			bytePtr[0]=(unsigned char)(tempVector.GetX()*255);
			bytePtr[1]=(unsigned char)(tempVector.GetY()*255);
			bytePtr[2]=(unsigned char)(tempVector.GetZ()*255);

			bytePtr+=3;
		}
	}
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
					0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	//negative y
	bytePtr=data;

	for(int j=0; j<size; j++)
	{
		for(int i=0; i<size; i++)
		{
			tempVector.SetX(i+offset-halfSize);
			tempVector.SetY(-halfSize);
			tempVector.SetZ(-(j+offset-halfSize));

			tempVector.Normalize();
			tempVector.PackTo01();

			bytePtr[0]=(unsigned char)(tempVector.GetX()*255);
			bytePtr[1]=(unsigned char)(tempVector.GetY()*255);
			bytePtr[2]=(unsigned char)(tempVector.GetZ()*255);

			bytePtr+=3;
		}
	}
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
					0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	//positive z
	bytePtr=data;

	for(int j=0; j<size; j++)
	{
		for(int i=0; i<size; i++)
		{
			tempVector.SetX(i+offset-halfSize);
			tempVector.SetY(-(j+offset-halfSize));
			tempVector.SetZ(halfSize);

			tempVector.Normalize();
			tempVector.PackTo01();

			bytePtr[0]=(unsigned char)(tempVector.GetX()*255);
			bytePtr[1]=(unsigned char)(tempVector.GetY()*255);
			bytePtr[2]=(unsigned char)(tempVector.GetZ()*255);

			bytePtr+=3;
		}
	}
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
					0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	//negative z
	bytePtr=data;

	for(int j=0; j<size; j++)
	{
		for(int i=0; i<size; i++)
		{
			tempVector.SetX(-(i+offset-halfSize));
			tempVector.SetY(-(j+offset-halfSize));
			tempVector.SetZ(-halfSize);

			tempVector.Normalize();
			tempVector.PackTo01();

			bytePtr[0]=(unsigned char)(tempVector.GetX()*255);
			bytePtr[1]=(unsigned char)(tempVector.GetY()*255);
			bytePtr[2]=(unsigned char)(tempVector.GetZ()*255);

			bytePtr+=3;
		}
	}
	glTexImage2D(	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,
					0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	delete [] data;

	return true;
}