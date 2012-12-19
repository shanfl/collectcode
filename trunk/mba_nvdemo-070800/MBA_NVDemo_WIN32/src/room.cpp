/**
 * room.cpp
 * Copyright 2000 by Mark B. Allan
 * 
 * draw the room.
 */
#include "room.h"

#include "winstuff.h"

#include <stdio.h>

//#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glpng.h>
#include <GL/glut.h>

#include "RglPrims.h"

static GLuint wallPaperTex = 0;
static GLuint floorTex = 0;
static GLuint muchaTex  = 0;
static GLuint mirrorTex  = 0;
static GLuint fLampShade = 0;
static GLuint fLampPost = 0;
static GLuint tblLampShade = 0;
static GLuint tblLampShade2 = 0;
static GLuint tblLampPost = 0;
static GLuint tblLampBase = 0;
static GLuint tblSurf1 = 0;
static GLuint tblSurf2 = 0;

static GLuint chairArm1_0 = 0;
static GLuint chairArm1_3 = 0;
static GLuint chairArm1_4 = 0;
static GLuint chairArm2_0 = 0;
static GLuint chairArm2_3 = 0;
static GLuint chairArm2_4 = 0;
static GLuint chairBack_0 = 0;
static GLuint chairBack_4 = 0;
static GLuint chairSeat_0 = 0;
static GLuint chairSeat_3 = 0;
static GLuint chairSeat_4 = 0;
static GLuint chairBase = 0;

//static GLuint clr_black		= 0;
//static GLuint clr_blue		= 0;
//static GLuint clr_gold		= 0;
//static GLuint clr_green		= 0;
//static GLuint clr_grey		= 0;
//static GLuint clr_purple	= 0;
//static GLuint clr_red		= 0;
static GLuint clr_white		= 0;

static GLuint fWallLm = 0;
static GLuint bWallLm = 0;
static GLuint lWallLm = 0;
static GLuint rWallLm = 0;
static GLuint ceilLm  = 0;
static GLuint floorLm = 0;

//static GLuint uvTex  = 0;

static GLuint sphereList = 0;
static GLuint glassList = 0;

static GLuint tableLampStemList		= 0;
static GLuint tableLampSurfList		= 0;
static GLuint tableLampTopList 		= 0;
static GLuint tableLampShadeList 	= 0;
static GLuint floorLampShadeList	= 0;
static GLuint floorLampStemList		= 0;
static GLuint ovalMirrorList		= 0;
static GLuint chairList				= 0;
static GLuint trashcanList			= 0;

bool roomListsExist = false;

extern float baseColor[4];
extern float baseRate;

#ifdef _WIN32
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
#endif
#define glTexCoord2f_M0(a, b) glMultiTexCoord2fARB(GL_TEXTURE0_ARB, a, b)
#define glTexCoord2f_M1(a, b) glMultiTexCoord2fARB(GL_TEXTURE1_ARB, a, b)

//------------------------------------------------
void loadRoomTextures()
{
	pngInfo tmpInfo;	
	fWallLm		= pngBind("PNG/wall000_lm.png",	  		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	bWallLm		= pngBind("PNG/wall180_lm.png",			PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	rWallLm		= pngBind("PNG/wall270_lm.png",			PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	lWallLm		= pngBind("PNG/wall090_lm.png",			PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	ceilLm		= pngBind("PNG/ceiling_lm.png",			PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	floorLm		= pngBind("PNG/floor_lm.png",			PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	muchaTex	= pngBind("PNG/mucha_blonde.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	mirrorTex	= pngBind("PNG/mirror.png",				PNG_BUILDMIPMAPS, PNG_ALPHA, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	fLampShade	= pngBind("PNG/floorLampShade.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	fLampPost	= pngBind("PNG/floorLampPost.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	tblLampBase	= pngBind("PNG/tableLampStem_tex.png",	PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	tblLampPost	= pngBind("PNG/tableLampStem_tex.png",	PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	tblLampShade= pngBind("PNG/tableLamp_tex.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	tblLampShade2= pngBind("PNG/tableLamp_tex2.png",	PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	tblSurf1	= pngBind("PNG/table_tex.png",			PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	tblSurf2	= pngBind("PNG/table_tex2.png",			PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);

	chairArm1_0 = pngBind("PNG/chair_arm1_0.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	chairArm1_3 = pngBind("PNG/chair_arm1_3.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	chairArm1_4 = pngBind("PNG/chair_arm1_4.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	chairArm2_0 = pngBind("PNG/chair_arm2_0.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	chairArm2_3 = pngBind("PNG/chair_arm2_3.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	chairArm2_4 = pngBind("PNG/chair_arm2_4.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	chairBack_0 = pngBind("PNG/chair_back_0.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	chairBack_4 = pngBind("PNG/chair_back_4.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	chairSeat_0 = pngBind("PNG/chair_seat_0.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	chairSeat_3 = pngBind("PNG/chair_seat_3.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	chairSeat_4 = pngBind("PNG/chair_seat_4.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	chairBase   = pngBind("PNG/chair_base.png",			PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);

//	uvTex		= pngBind("PNG/uv.png", 				PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_CLAMP, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	
	floorTex	= pngBind("PNG/floor_tex.png",			PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_REPEAT,  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	wallPaperTex= pngBind("PNG/wallpaper_tex.png",		PNG_BUILDMIPMAPS, PNG_SOLID, &tmpInfo, GL_REPEAT,  GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR);
	
//	clr_black	= pngBind("PNG/clr_black.png",			PNG_NOMIPMAP, PNG_SOLID, &tmpInfo, GL_CLAMP,  GL_NEAREST, GL_NEAREST);
//	clr_blue	= pngBind("PNG/clr_blue.png",			PNG_NOMIPMAP, PNG_SOLID, &tmpInfo, GL_CLAMP,  GL_NEAREST, GL_NEAREST);
//	clr_gold	= pngBind("PNG/clr_gold.png",			PNG_NOMIPMAP, PNG_SOLID, &tmpInfo, GL_CLAMP,  GL_NEAREST, GL_NEAREST);
//	clr_green	= pngBind("PNG/clr_green.png",			PNG_NOMIPMAP, PNG_SOLID, &tmpInfo, GL_CLAMP,  GL_NEAREST, GL_NEAREST);
//	clr_grey	= pngBind("PNG/clr_grey.png",			PNG_NOMIPMAP, PNG_SOLID, &tmpInfo, GL_CLAMP,  GL_NEAREST, GL_NEAREST);
//	clr_purple	= pngBind("PNG/clr_purple.png",			PNG_NOMIPMAP, PNG_SOLID, &tmpInfo, GL_CLAMP,  GL_NEAREST, GL_NEAREST);
//	clr_red		= pngBind("PNG/clr_red.png",			PNG_NOMIPMAP, PNG_SOLID, &tmpInfo, GL_CLAMP,  GL_NEAREST, GL_NEAREST);
	clr_white	= pngBind("PNG/clr_white.png",			PNG_NOMIPMAP, PNG_SOLID, &tmpInfo, GL_CLAMP,  GL_NEAREST, GL_NEAREST);

	if(!fWallLm)
		fprintf(stderr, "WARNING: textures must be in ./PNG\n");
		
	createLists();
}

//------------------------------------------------
void deleteRoomTextures()
{
	glDeleteTextures(1, &fWallLm	  );
	glDeleteTextures(1, &bWallLm	  );
	glDeleteTextures(1, &rWallLm	  );
	glDeleteTextures(1, &lWallLm	  );
	glDeleteTextures(1, &ceilLm 	  );
	glDeleteTextures(1, &floorLm	  );
	glDeleteTextures(1, &muchaTex	  );
	glDeleteTextures(1, &mirrorTex	  );
	glDeleteTextures(1, &fLampShade   );
	glDeleteTextures(1, &fLampPost    );
	glDeleteTextures(1, &tblLampBase  );
	glDeleteTextures(1, &tblLampPost  );
	glDeleteTextures(1, &tblLampShade );
	glDeleteTextures(1, &tblLampShade2);
	glDeleteTextures(1, &tblSurf1     );
	glDeleteTextures(1, &tblSurf2     );

	glDeleteTextures(1, &chairArm1_0  );
	glDeleteTextures(1, &chairArm1_3  );
	glDeleteTextures(1, &chairArm1_4  );
	glDeleteTextures(1, &chairArm2_0  );
	glDeleteTextures(1, &chairArm2_3  );
	glDeleteTextures(1, &chairArm2_4  );
	glDeleteTextures(1, &chairBack_0  );
	glDeleteTextures(1, &chairBack_4  );
	glDeleteTextures(1, &chairSeat_0  );
	glDeleteTextures(1, &chairSeat_3  );
	glDeleteTextures(1, &chairSeat_4  );
	glDeleteTextures(1, &chairBase    );

//	glDeleteTextures(1, &uvTex  	  );

	glDeleteTextures(1, &floorTex	  );
	glDeleteTextures(1, &wallPaperTex );

//	glDeleteTextures(1, &clr_black	  );
//	glDeleteTextures(1, &clr_blue	  );
//	glDeleteTextures(1, &clr_gold	  );
//	glDeleteTextures(1, &clr_green	  );
//	glDeleteTextures(1, &clr_grey	  );
//	glDeleteTextures(1, &clr_purple   );
//	glDeleteTextures(1, &clr_red 	  );
	glDeleteTextures(1, &clr_white	  );
}


//------------------------------------------------
void reloadRoomTextures()
{
	deleteRoomTextures();
	loadRoomTextures();
}

//------------------------------------------------
void drawFrontWall()
{	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, wallPaperTex);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, fWallLm);
	
	//-- front wall
	glBegin(GL_QUADS);
		glTexCoord2f_M0( 1.0,  0.0);	
		glTexCoord2f_M1(10.0,  0.0);
		glVertex3f( 7.0,  8.0, -7.0);
		glTexCoord2f_M0( 0.0,  0.0);	
		glTexCoord2f_M1( 0.0,  0.0);
		glVertex3f(-7.0,  8.0, -7.0);
		glTexCoord2f_M0( 0.0,  1.0);	
		glTexCoord2f_M1( 0.0,  6.0);
		glVertex3f(-7.0,  0.0, -7.0);
		glTexCoord2f_M0( 1.0,  1.0);	
		glTexCoord2f_M1(10.0,  6.0);
		glVertex3f( 7.0,  0.0, -7.0);
	glEnd();
}

//------------------------------------------------
void drawRoomWalls()
{	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, wallPaperTex);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	
	//-- back wall
	glBindTexture(GL_TEXTURE_2D, bWallLm);
	glBegin(GL_QUADS);
		glTexCoord2f_M0( 1.0, 0.0);	
		glTexCoord2f_M1(10.0, 0.0);
		glVertex3f( 7.0,  8.0,  12.0);
		glTexCoord2f_M0( 1.0, 1.0);	
		glTexCoord2f_M1(10.0, 6.0);
		glVertex3f( 7.0,  0.0,  12.0);
		glTexCoord2f_M0( 0.0, 1.0);	
		glTexCoord2f_M1( 0.0, 6.0);
		glVertex3f(-7.0,  0.0,  12.0);
		glTexCoord2f_M0( 0.0, 0.0);	
		glTexCoord2f_M1( 0.0, 0.0);
		glVertex3f(-7.0,  8.0,  12.0);
	glEnd();

	//-- right wall
	glBindTexture(GL_TEXTURE_2D, rWallLm);
	glBegin(GL_QUADS);
		glTexCoord2f_M0( 0.0, 0.0);	
		glTexCoord2f_M1( 0.0, 0.0);
		glVertex3f( 7.0,  8.0,  -7.0);
		glTexCoord2f_M0( 0.0, 1.0);	
		glTexCoord2f_M1( 0.0, 6.0);
		glVertex3f( 7.0,  0.0,  -7.0);
		glTexCoord2f_M0( 1.0, 1.0);	
		glTexCoord2f_M1(14.0, 6.0);
		glVertex3f( 7.0,  0.0,  12.0);
		glTexCoord2f_M0( 1.0, 0.0);	
		glTexCoord2f_M1(14.0, 0.0);
		glVertex3f( 7.0,  8.0,  12.0);
	glEnd();

	//-- left wall
	glBindTexture(GL_TEXTURE_2D, lWallLm);
	glBegin(GL_QUADS);
		glTexCoord2f_M0( 0.0, 0.0);	
		glTexCoord2f_M1( 0.0, 0.0);
		glVertex3f(-7.0,  8.0,  12.0);
		glTexCoord2f_M0( 0.0, 1.0);	
		glTexCoord2f_M1( 0.0, 6.0);
		glVertex3f(-7.0,  0.0,  12.0);
		glTexCoord2f_M0( 1.0, 1.0);	
		glTexCoord2f_M1(14.0, 6.0);
		glVertex3f(-7.0,  0.0,  -7.0);
		glTexCoord2f_M0( 1.0, 0.0);	
		glTexCoord2f_M1(14.0, 0.0);
		glVertex3f(-7.0,  8.0,  -7.0);
	glEnd();
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, clr_white);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	
	//-- ceiling
	glBindTexture(GL_TEXTURE_2D, ceilLm);
	glBegin(GL_QUADS);
		glTexCoord2f_M0(0.0, 1.0);	
		glTexCoord2f_M1(0.0, 1.0);	
		glVertex3f(-7.0,  8.0,  -7.0);
		glTexCoord2f_M0(1.0, 1.0);	
		glTexCoord2f_M1(1.0, 1.0);	
		glVertex3f( 7.0,  8.0,  -7.0);
		glTexCoord2f_M0(1.0, 0.0);	
		glTexCoord2f_M1(1.0, 0.0);	
		glVertex3f( 7.0,  8.0,  12.0);
		glTexCoord2f_M0(0.0, 0.0);	
		glTexCoord2f_M1(0.0, 0.0);	
		glVertex3f(-7.0,  8.0,  12.0);
	glEnd();
	
}

//------------------------------------------------
void drawFloor(float alpha)
{
	glColor4f(1.0, 1.0, 1.0, alpha);
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, floorTex);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, floorLm);
	
	//-- floor
	glBegin(GL_QUADS);
		glTexCoord2f_M0(0.0, 1.0);	
		glTexCoord2f_M1(0.0, 2.0);
		glVertex3f(-7.0,  0.0,  12.0);
		glTexCoord2f_M0(1.0, 1.0);	
		glTexCoord2f_M1(3.0, 2.0);
		glVertex3f( 7.0,  0.0,  12.0);
		glTexCoord2f_M0(1.0, 0.0);	
		glTexCoord2f_M1(3.0, 0.0);
		glVertex3f( 7.0,  0.0,  -7.0);
		glTexCoord2f_M0(0.0, 0.0);	
		glTexCoord2f_M1(0.0, 0.0);
		glVertex3f(-7.0,  0.0,  -7.0);
	glEnd();
	
	glColor4fv(baseColor);	
}

//------------------------------------------------
void drawChair()
{
	GLuint texList[6];
	MVertex c, v1, v2;
	
	texList[0] = chairBase;   // top
	texList[1] = chairBase;   // bottom;
	texList[2] = chairBase;	  // left;
	texList[3] = chairBase;	  // front;
	texList[4] = chairBase;	  // right;
	texList[5] = chairBase;   // back;
	
	//-- arm
	texList[0] = chairArm1_0;
	texList[3] = chairArm1_3;
	texList[4] = chairArm1_4;
	c.set (-4.8, 2.0, -6.0);
	v1.set(0.8, 0.0, 0.0);
	v2.set(0.0, 0.0, 2.2);
	RglCube(c, v1, v2, 2.0, 1, texList);
	//-- arm
	texList[0] = chairArm2_0;
	texList[3] = chairArm2_3;
	texList[4] = chairArm2_4;
	c.set (-1.8, 2.0, -6.0);
	v1.set(0.8, 0.0, 0.0);
	v2.set(0.0, 0.0, 2.2);
	RglCube(c, v1, v2, 2.0, 1, texList);
	//-- seat
	texList[0] = chairSeat_0;
	texList[3] = chairSeat_3;
	texList[4] = chairSeat_4;
	c.set (-4.2, 1.4, -5.0);
	v1.set(2.6, 0.0, 0.0);
	v2.set(0.0, 0.3, 2.0);
	RglCube(c, v1, v2, 0.8, 1, texList);
	//-- back
	texList[0] = chairBack_0;
	texList[4] = chairBack_4;
	c.set (-4.2, 3.2, -5.8);
	v1.set(2.6,  0.0, 0.0);
	v2.set(0.0, -1.8, 1.0);
	RglCube(c, v1, v2, 0.8, 1, texList);
}

//------------------------------------------------
void drawRoomObjects()
{
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, clr_white);
	glActiveTextureARB(GL_TEXTURE0_ARB);

	//-- draw chair
	glCallList(chairList);
	
	//-- draw table lamp desk surface
	glCallList(tableLampSurfList);

	//-- table lamp shade
	glCallList(tableLampShadeList);
}

/*------------------------------------*/
void drawMirror(bool alpha)
{
	if(alpha)
		glColor4f(0.4, 0.4, 0.5, 0.3);
	else
		glColor4f(0.6, 0.6, 0.7, 1.0);
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, clr_white);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, mirrorTex);
	glPushMatrix();
		glTranslatef( -2.0, 5.2, -6.9 );
		glCallList(ovalMirrorList);
	glPopMatrix();

	//-- mucha
	if(alpha)
		glColor4f(1.0, 1.0, 1.0, 0.7);
	else
		glColor4f(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, muchaTex);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0);	glVertex3f( 3.0,  4.0,  -6.9);
		glTexCoord2f(1.0, 1.0);	glVertex3f( 4.6,  4.0,  -6.9);
		glTexCoord2f(1.0, 0.0);	glVertex3f( 4.6,  6.0,  -6.8);
		glTexCoord2f(0.0, 0.0);	glVertex3f( 3.0,  6.0,  -6.8);
	glEnd();
	
	glColor4fv(baseColor);
}

/*------------------------------------*/
void drawReflect(MVertex &e, MVertex &scale)
{
	static float dcount = 1.0;
	dcount += baseRate;
	
	MVertex up(0.0, 1.0, 0.0);
	MVertex eye(e);
//	scale.scale(up);
//	scale.scale(eye);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, clr_white);	
		
	
	MMatrix	mat;
	MVertex v1;
	
	glEnable(GL_TEXTURE_CUBE_MAP_EXT);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	
	//-- begin sphere 
	float speed = 0.03;
	float bys;
	float byc;
	MVertex		pos, cpos;
	
	//-- Center sphere texture setup
	bys 	= fabs(3.5*sin(dcount*speed));
	cpos = pos.set( 0.0,  1.5+bys, -1.5 );
	scale.scale(cpos);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix(); //=GL_TEXTURE
	glScalef(scale.x(), scale.y(), scale.z());
	// KLUDGE the ball in mirror reflection
	if(scale.z() < 0)
	{
		eye.setX(-eye.x());
		cpos.setZ(-8.5);
	}
	v1.set(eye-cpos);
	mat.pointAt(v1, up);
	glMultMatrixf(mat.mat());	
			//  Center sphere geometry call
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();	//=GL_MODELVIEW
				glTranslatef( pos.x(), pos.y(), pos.z() );
				glCallList(sphereList);
			glPopMatrix();	//=GL_MODELVIEW
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();	//=GL_TEXTURE

	//----------------------------------
	bys 	= 0.3*sin(dcount*speed*0.75);
	byc 	= 0.5*cos(dcount*speed*2.0);
	//-- Left side Sphere texture setup
	pos.set ( -4.5+bys,  3.5+byc, -1.5 );
	cpos.set( -9.0+bys,  3.5+byc, -1.5 );// tweak the view
	scale.scale(cpos);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();	//=GL_TEXTURE
	glScalef(scale.x(), -scale.y(), scale.z());	//-- flip Y for glass
	v1.set(eye-cpos);
	mat.pointAt(v1, up);
	glMultMatrixf(mat.mat());	
			//  Center sphere geometry call
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();	//=GL_MODELVIEW
				glTranslatef( pos.x(), pos.y(), pos.z() );
				glRotatef(dcount*5.0, -0.5, 1.0+bys, 0.0);
				glCallList(glassList);
			glPopMatrix();	//=GL_MODELVIEW
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();	//=GL_TEXTURE

	//-- Right side Sphere texture setup
	pos.set (4.5-bys,  3.5+byc, -1.5 );
	cpos.set(9.0-bys,  3.5+byc, -1.5 );// tweak the view
	scale.scale(cpos);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();	//=GL_TEXTURE
	glScalef(scale.x(), -scale.y(), scale.z());	//-- flip Y for glass
	v1.set(eye-cpos);
	mat.pointAt(v1, up);
	glMultMatrixf(mat.mat());	
			//  Center sphere geometry call
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();	//=GL_MODELVIEW
				glTranslatef( pos.x(), pos.y(), pos.z() );
				glRotatef(dcount*5.0, 0.5, 1.0+bys, 0.0);
				glCallList(glassList);
			glPopMatrix();	//=GL_MODELVIEW
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();	//=GL_TEXTURE
	
	glPushMatrix();	//=GL_TEXTURE
	//-- begin table lamp stem
//	v1.set(eye-MVertex(-3.3,  0.0, 3.0));
	v1.set(eye-MVertex( 0.0,  0.0, 0.0));
	mat.pointAt(v1, up);
	glMultMatrixf(mat.mat());
	
//	glTranslatef( 0.0, -0.4, 0.0);
glMatrixMode(GL_MODELVIEW);
	//-- floor lamp stem
	glCallList(floorLampStemList);
//glMatrixMode(GL_TEXTURE);
//	glTranslatef(0.0, 0.8, 0.0);
glMatrixMode(GL_MODELVIEW);
	//-- floor lamp shade
	glCallList(floorLampShadeList);
	
	//-- table lamp top
	glCallList(tableLampTopList);
	//-- table lamp stem
	glColor4f(0.7, 0.7, 0.7, 1.0);
	glCallList(tableLampStemList);
	
	//-- trashcan
	glColor4f(0.5, 0.5, 0.5, 1.0);
	glCallList(trashcanList);

	glMatrixMode(GL_TEXTURE);
	glPopMatrix();	//=GL_TEXTURE
	glMatrixMode(GL_MODELVIEW);
	
	glDisable(GL_TEXTURE_CUBE_MAP_EXT);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	
	glColor4fv(baseColor);
}

//----------------------------------------------------------
void genRoomLists()
{
	if(!roomListsExist)
	{
		sphereList			= glGenLists(1);
		glassList			= glGenLists(1);
		tableLampStemList	= glGenLists(1);
		tableLampShadeList	= glGenLists(1);
		tableLampTopList	= glGenLists(1);
		tableLampSurfList	= glGenLists(1);
		floorLampShadeList	= glGenLists(1);
		floorLampStemList	= glGenLists(1);
		ovalMirrorList		= glGenLists(1);
		trashcanList		= glGenLists(1);
		chairList			= glGenLists(1);
		roomListsExist = true;
	}
}

//----------------------------------------------------------
void delRoomLists()
{
	if(roomListsExist)
	{
		glDeleteLists(sphereList	  	, 1);
		glDeleteLists(glassList 	  	, 1);
		glDeleteLists(tableLampStemList , 1);
		glDeleteLists(tableLampShadeList, 1);
		glDeleteLists(tableLampTopList	, 1);
		glDeleteLists(tableLampSurfList , 1);
		glDeleteLists(floorLampShadeList, 1);
		glDeleteLists(floorLampStemList , 1);
		glDeleteLists(ovalMirrorList  	, 1);
		glDeleteLists(trashcanList    	, 1);
		glDeleteLists(chairList 	  	, 1);
		sphereList  	   = 0;
		glassList		   = 0;
		tableLampStemList  = 0;
		tableLampShadeList = 0;
		tableLampTopList   = 0;
		tableLampSurfList  = 0;
		floorLampShadeList = 0;
		floorLampStemList  = 0;
		ovalMirrorList     = 0;
		trashcanList	   = 0;
		chairList		   = 0;
		roomListsExist = false;
	}
}

//----------------------------------------------------------
void createLists()
{
	GLuint texList[6];
	MVertex c, v1, v2;
	
	if(!roomListsExist)
	{
		genRoomLists();	
	}
	
	//-- reflect sphere
	glNewList(sphereList, GL_COMPILE);
	RglSphere  (1.5,  -1.5,   1.5, 360.0, 32);
	glEndList();

	//-- refract sphere
	MVertex tmp(0.5, 0.5, 0.5);
	glNewList(glassList, GL_COMPILE);
	RglSphereN  (1.0,  -1.0,   1.0, 360.0, 24, tmp);
	glEndList();

	//-- table lamp stem
	tmp.set(1.0, 1.0, 0.4);
	glNewList(tableLampStemList, GL_COMPILE);
		glPushMatrix();
			glTranslatef(1.75,  0.0, -4.5);
			glRotatef(-90.0, 1.0, 0.0, 0.0);
			RglCylinder(0.075, 0.0, 4.9, 360.0, 8, false);
			glScalef(1.0, 1.0, 0.2);
			RglSphereN(0.8, 0.0, 0.8, 360.0, 16, tmp);
		glPopMatrix();
	glEndList();
	
	//-- table lamp shade
	glNewList(tableLampShadeList, GL_COMPILE);
		glPushMatrix();
			glBindTexture(GL_TEXTURE_2D, tblLampPost);
			glTranslatef(1.75,  4.86, -4.5);
			glRotatef(-90.0, 1.0, 0.0, 0.0);
			//-- Rely on fortune and luck here to make the transparency work ;)
			glTranslatef(0.0, 0.0, 0.14);
			glBindTexture(GL_TEXTURE_2D, clr_white);
			RglSphere(0.14, -0.14, 0.14, 360.0, 10); // lightbulb
			glColor4f(1.0, 0.98, 0.95, 0.9);
			glBindTexture(GL_TEXTURE_2D, tblLampShade2);
			glScalef(1.0, 1.0, 0.7);
			RglSphere(0.7, -0.35, 0.6, -360.0, 24); // backface
			glBindTexture(GL_TEXTURE_2D, tblLampShade);
			RglSphere(0.7, -0.35, 0.6,  360.0, 24);
			glColor4fv(baseColor);
		glPopMatrix();
	glEndList();
	
	//-- table lamp shade top 
	glNewList(tableLampTopList, GL_COMPILE);
		glPushMatrix();
			glTranslatef(1.75,  4.86, -4.5);
			glRotatef(-90.0, 1.0, 0.0, 0.0);
			RglSphere(0.69, 0.49, 0.67, 360.0, 24);
		glPopMatrix();
	glEndList();
	
	//-- table lamp desk surface
	texList[0] = tblSurf1;
	texList[1] = texList[2] = texList[3] = texList[4] = texList[5] = tblSurf2;
	c.set (0.75, 3.0, -5.5);
	v1.set( 2.0, 0.0, 0.0);
	v2.set( 0.0, 0.0, 2.0);
	glNewList(tableLampSurfList, GL_COMPILE);
		RglCube(c, v1, v2, 0.1, 1, texList);
	glEndList();
	
	//-- floor lamp shade
	glNewList(floorLampShadeList, GL_COMPILE);
		glPushMatrix();
			glTranslatef(-3.3,  6.35, 3.0);
			glRotatef(90.0, 1.0, 0.0, 0.0);
			RglCone(1.0, 1.0,  360.0, 24, false);
			RglCone(1.0, 1.0, -360.0, 24, false);	// draw the back facing surface
		glPopMatrix();
	glEndList();
	
	//-- floor lamp stem and base
	glNewList(floorLampStemList, GL_COMPILE);
		glPushMatrix();
			glTranslatef(-3.3,  0.0, 3.0);
			glRotatef(-90.0, 1.0, 0.0, 0.0);
			RglCylinder(0.1, 0.0, 5.5, 360.0, 8, false);
			glScalef(1.0, 1.0, 0.2);
			RglSphereN(0.9, 0.0, 0.9, 360.0, 16, tmp);
		glPopMatrix();
	glEndList();
	
	//-- oval mirror
	glNewList(ovalMirrorList, GL_COMPILE);
		glPushMatrix();
//			glTranslatef(-2.0, 5.2, -6.9);
			glScalef(1.0, 0.6, 0.05);
		//	RglCylinder(2.0, 0.0, 0.1, 360.0, 32, false);
			RglDisk(0.1, 2.0, 360.0, 32, false);
		glPopMatrix();
	glEndList();
	
	//-- trashcan
	glNewList(trashcanList, GL_COMPILE);
		glPushMatrix();
			glTranslatef( 3.8,  0.0, -4.5);
			glRotatef(-90.0, 1.0, 0.0, 0.0);
			RglCylinder(0.52, 0.0, 1.2,  360.0, 12, false);
			RglCylinder(0.50, 0.0, 1.2, -360.0, 12, false); // draw the back facing surface
		glPopMatrix();
	glEndList();
	
	//-- chair
	glNewList(chairList, GL_COMPILE);
		drawChair();
	glEndList();
	
}

