//////////////////////////////////////////////////////////////////////////////////////////
//	Simple Bumpmapping
//	Accompanies a tutorial found on my site
//	Downloaded from: www.paulsprojects.net
//	Created:	6th December 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include "Extensions/ARB_multitexture_extension.h"
#include "Extensions/ARB_texture_cube_map_extension.h"
#include "Extensions/ARB_texture_env_combine_extension.h"
#include "Extensions/ARB_texture_env_dot3_extension.h"
#include "Image/IMAGE.h"
#include "Maths/Maths.h"
#include "TORUS.h"
#include "Normalisation Cube Map.h"
#include "main.h"
#include <stdlib.h>
//Our torus
TORUS torus;

//Normal map
GLuint normalMap;

//Decal texture
GLuint decalTexture;

//Normalisation cube map
GLuint normalisationCubeMap;

//Light position in world space
VECTOR3D worldLightPosition=VECTOR3D(10.0f, 10.0f, 10.0f);

bool drawBumps=true;
bool drawColor=true;

//Called for initiation
void Init(void)
{
	//Check for and set up extensions
	if(	!SetUpARB_multitexture()		||	!SetUpARB_texture_cube_map()	||
		!SetUpARB_texture_env_combine()	||	!SetUpARB_texture_env_dot3())
	{
		printf("Required Extension Unsupported\n");
		exit(0);
	}

	//Load identity modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Shading states
	glShadeModel(GL_SMOOTH);
	glClearColor(0.2f, 0.4f, 0.2f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//Depth states
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

	//Load normal map
	IMAGE normalMapImage;
	normalMapImage.Load("Normal map.bmp");
	normalMapImage.ExpandPalette();
	
	//Convert normal map to texture
	glGenTextures(1, &normalMap);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGBA8, normalMapImage.width, normalMapImage.height,
					0, normalMapImage.format, GL_UNSIGNED_BYTE, normalMapImage.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Load decal image
	IMAGE decalImage;
	decalImage.Load("decal.bmp");
	decalImage.ExpandPalette();
	
	//Convert decal image to texture
	glGenTextures(1, &decalTexture);
	glBindTexture(GL_TEXTURE_2D, decalTexture);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGBA8, decalImage.width, decalImage.height,
					0, decalImage.format, GL_UNSIGNED_BYTE, decalImage.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//Create normalisation cube map
	glGenTextures(1, &normalisationCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, normalisationCubeMap);
	GenerateNormalisationCubeMap();
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_NORMAL_MAP_ARB);
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_NORMAL_MAP_ARB);
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_NORMAL_MAP_ARB);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

//Called to draw scene
void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//use gluLookAt to look at torus
	gluLookAt(	0.0f,10.0f,10.0f, 
				0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f);

	//rotate torus
	static float angle=0.0f;
	angle+=0.1f;
	glRotatef(angle, 0.0f, 1.0f, 0.0f);


	//Get the inverse model matrix
	MATRIX4X4 inverseModelMatrix;
	glPushMatrix();
	glLoadIdentity();
	glRotatef(-angle, 0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, inverseModelMatrix);
	glPopMatrix();

	//Get the object space light vector
	VECTOR3D objectLightPosition=inverseModelMatrix*worldLightPosition;
	
	//Loop through vertices
	for(int i=0; i<torus.numVertices; ++i)
	{
		VECTOR3D lightVector=objectLightPosition-torus.vertices[i].position;
		
		//Calculate tangent space light vector
		torus.vertices[i].tangentSpaceLight.x=
			torus.vertices[i].sTangent.DotProduct(lightVector);
		torus.vertices[i].tangentSpaceLight.y=
			torus.vertices[i].tTangent.DotProduct(lightVector);
		torus.vertices[i].tangentSpaceLight.z=
			torus.vertices[i].normal.DotProduct(lightVector);
	}


	//Draw bump pass
	if(drawBumps)
	{
		//Bind normal map to texture unit 0
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glEnable(GL_TEXTURE_2D);

		//Bind normalisation cube map to texture unit 1
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, normalisationCubeMap);
		glEnable(GL_TEXTURE_CUBE_MAP_ARB);
		glActiveTextureARB(GL_TEXTURE0_ARB);

		//Set vertex arrays for torus
		glVertexPointer(3, GL_FLOAT, sizeof(TORUS_VERTEX), &torus.vertices[0].position);
		glEnableClientState(GL_VERTEX_ARRAY);

		//Send texture coords for normal map to unit 0
		glTexCoordPointer(2, GL_FLOAT, sizeof(TORUS_VERTEX), &torus.vertices[0].s);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//Send tangent space light vectors for normalisation to unit 1
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glTexCoordPointer(3, GL_FLOAT, sizeof(TORUS_VERTEX), &torus.vertices[0].tangentSpaceLight);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTextureARB(GL_TEXTURE0_ARB);


		//Set up texture environment to do (tex0 dot tex1)*color
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);

		glActiveTextureARB(GL_TEXTURE1_ARB);
	
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_DOT3_RGB_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);
	
		glActiveTextureARB(GL_TEXTURE0_ARB);


	
		//Draw torus
		glDrawElements(GL_TRIANGLES, torus.numIndices, GL_UNSIGNED_INT, torus.indices);


		//Disable textures
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glActiveTextureARB(GL_TEXTURE0_ARB);

		//disable vertex arrays
		glDisableClientState(GL_VERTEX_ARRAY);
	
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
		glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTextureARB(GL_TEXTURE0_ARB);

		//Return to standard modulate texenv
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	//If we are drawing both passes, enable blending to multiply them together
	if(drawBumps && drawColor)
	{
		//Enable multiplicative blending
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glEnable(GL_BLEND);
	}

	//Perform a second pass to color the torus
	if(drawColor)
	{
		if(!drawBumps)
		{
			glLightfv(GL_LIGHT1, GL_POSITION, VECTOR4D(objectLightPosition));
			glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
			glLightfv(GL_LIGHT1, GL_AMBIENT, black);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);
			glEnable(GL_LIGHT1);
			glEnable(GL_LIGHTING);

			glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
		}

		//Bind decal texture
		glBindTexture(GL_TEXTURE_2D, decalTexture);
		glEnable(GL_TEXTURE_2D);

		//Set vertex arrays for torus
		glVertexPointer(3, GL_FLOAT, sizeof(TORUS_VERTEX), &torus.vertices[0].position);
		glEnableClientState(GL_VERTEX_ARRAY);

		glNormalPointer(GL_FLOAT, sizeof(TORUS_VERTEX), &torus.vertices[0].normal);
		glEnableClientState(GL_NORMAL_ARRAY);

		glTexCoordPointer(2, GL_FLOAT, sizeof(TORUS_VERTEX), &torus.vertices[0].s);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//Draw torus
		glDrawElements(GL_TRIANGLES, torus.numIndices, GL_UNSIGNED_INT, torus.indices);

		if(!drawBumps)
			glDisable(GL_LIGHTING);

		//Disable texture
		glDisable(GL_TEXTURE_2D);

		//disable vertex arrays
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	//Disable blending if it is enabled
	if(drawBumps && drawColor)
		glDisable(GL_BLEND);

	glFinish();
	glutSwapBuffers();
	glutPostRedisplay();
}

//Called on window resize
void Reshape(int w, int h)
{
	//Set viewport
	if(h==0)
		h=1;

	glViewport(0, 0, w, h);

	//Set up projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(	45.0f, (GLfloat)w/(GLfloat)h, 1.0f, 100.0f);
	
	glMatrixMode(GL_MODELVIEW);
}

//Called when a key is pressed
void Keyboard(unsigned char key, int x, int y)
{
	//If escape is pressed, exit
	if(key==27)
		exit(0);

	//'1' draws both passes
	if(key=='1')
	{
		drawBumps=true;
		drawColor=true;
	}

	//'2' draws only bumps
	if(key=='2')
	{
		drawBumps=true;
		drawColor=false;
	}

	//'3' draws only color
	if(key=='3')
	{
		drawBumps=false;
		drawColor=true;
	}

	//'W' draws in wireframe
	if(key=='W' || key=='w')
		glPolygonMode(GL_FRONT, GL_LINE);

	//'F' return to fill
	if(key=='F' || key=='f')
		glPolygonMode(GL_FRONT, GL_FILL);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Simple Bumpmapping");

	Init();

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
	return 0;
}