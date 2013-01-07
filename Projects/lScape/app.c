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
	File:		app.c

	Function:	geoMipmap test app
*/

#include "opengl.h"
#include "main.h"
#include "camera.h"
#include "frustum.h"
#include "boundbox.h"
#include "callback.h"
#include "freeCamera.h"
#include "textureManager.h"
#include "textureFont.h"
#include "lscape.h"
#include "tgafile.h"

/*
 * Timer for fps
 */
static int timerBegin;
static int timerElapsed;
static int timerFrame;
static int frameCount;

/* #define TIMER_DETAILED */

#ifdef TIMER_DETAILED
static LARGE_INTEGER timerDetail[5];
#endif

/*
 * Texture mapped font
 */
static tmf myFont;

/*
 * Landscape
 */
static unsigned char *lscHm = NULL;
/*static vec3 lscScale = {100, 100, 10};*/
static vec3 lscScale = {50, 50, 10};
/*static vec3 lscScale = {25, 25, 10};*/
/*static int lscHmSz = 64;*/
static int lscHmSz = 512;
/*static int lscHmSz = 1024;*/
/*static int lscPatchSz = 8;*/
static int lscPatchSz = 16;
/* Note: repeat must be power of tool or else quadtree will crash */
static int lscRepeat = 2;
static lsc lscape = NULL;
static bool sharing = false;

static int lscPatchVtxCchSz = 4 * 1024;
static int lscPatchIdxCchSz = 1024;

/*
 * Landscape Texture
 */
/*static int lscTexSize = 2048;*/
/*static int lscTexTile = 1;*/
/*static int lscTexSize = 256;*/
/*static int lscTexTile = 8;*/
static int lscTexSize = 512;
static int lscTexTile = 1;
static int lscBaseTexTile = 64;

/*
 * Lanscape Occlusion
 */
static int  lscOccSectors = 64;
/*static int  lscOccPatchSize = 16;*/
static int  lscOccPatchSize = 32;
/*static int  lscOccPatchSize = 64;*/
static int  lscOccMaxOccPts = 8;
static bool lscOccDisable = false;

/*
 * Fog
 */
static int   fogMode = GL_LINEAR;
static float fogColor[4] = {0.5f, 0.5f, 0.5f, 1.0f};

/*
 * View
 */
static float nearp = 5.0f;
static bool	 godView = false;

/*
 * Current drawing mode.
 * 0 = line,  1 = texture, 2 = texture splat, 3 = texture splat with CVAs
 */
static bool lscMode = 2;

/*
 * Check for collisions
 */
static bool lscCollisionDetect = false;

/*
 * Draw info messages?
 */
static bool lscDrawInfo = false;

/*
 * Use texture_env_combine if available
 */
static bool lscUseTextureCombine = true;

/*
 * Draw any messages in the top left of the window
 */
void drawInfo(void)
{
	static char msgbf[][64] = {	"xxxx fps",
								"Mode: texture splat, CVAxxx",
								"TextureCombine: offxxx",
								"Occlusion: offxxx",
								"Collision Detect offxxx",
								""	};
	static int fps = 0;

	char *msg[7];
	int i;

	for (i = 0; i< 6; i++)
		msg[i] = msgbf[i];
	msg[i] = NULL;

	if (!lscDrawInfo)
		return;

	if ((timerElapsed - timerFrame) > 1000) {
		fps = (int)((1000.0f * frameCount) / (float)(timerElapsed - timerFrame));
		timerFrame = timerElapsed;
		frameCount = 0;
		if (fps > 9999)
			fps = 9999;
		else if (fps < 0)
			fps = 0;
	}

	if (myFont) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, win_width, win_height, 0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		sprintf(msg[0], "%d fps", fps);
		switch(lscMode) {
		default:
		case 0:
			sprintf(msg[1], "Mode: line");
			break;
		case 1:
			sprintf(msg[1], "Mode: textured");
			break;
		case 2:
			sprintf(msg[1], "Mode: texture splat");
			break;
		case 3:
			sprintf(msg[1], "Mode: texture splat, CVA");
			break;
		}
		sprintf(msg[2], "TextureCombine: %s",
				(lscMode > 1) && lscUseTextureCombine ? "on" : "off");
		sprintf(msg[3], "Occlusion: %s", lscOccDisable ? "off" : "on");
		sprintf(msg[4], "Collision Detect: %s", lscCollisionDetect ? "on" : "off");
		if (lscape && lscape->patchVtxCch.fast)
			msg[5] = "Using fast memory & NV VAR Fence";
		else
			msg[5] = NULL;
		tmf_drawStrings(myFont, 0, 0, msg, 1.0f);
	}
}

/*
 * Load the height map into memory
 */
unsigned char *loadHeightmap(void)
{
	char fileName[30];
	tga tgaFile;
	unsigned char *heightMap = NULL;
	int i;

	sprintf(fileName, "data/Height%d.tga", lscHmSz);
	tgaFile = tga_create(fileName);
	if (!tgaFile)
		return heightMap;
	if (!tgaFile->data) {
		tga_destroy(tgaFile);
		return heightMap;
	}

	heightMap = (unsigned char*)malloc(
					sizeof(unsigned char) * SQR(lscHmSz + 1));
	if (heightMap) {

		for (i = 0; i <= lscHmSz; i++) {
			memcpy(heightMap + i*(lscHmSz + 1),
					tgaFile->data + i*lscHmSz,
					lscHmSz);
			/* The first shall be last! */
			*(heightMap + (i + 1)*(lscHmSz + 1) - 1) =
				*(heightMap + i*(lscHmSz + 1));
		}

		/* The first shall be last! */
		memcpy(heightMap + lscHmSz * (lscHmSz + 1),
				heightMap,
				lscHmSz + 1);
	}

	tga_destroy(tgaFile);

	return heightMap;
}

void lscCleanup()
{
	if (lscHm)
		free(lscHm);
	lscHm = NULL;
	txm_deleteTextures();
	cam_destroy(cam_getGlobalCamera());
	if (!sharing)
		if (lscape)
			lsc_destroy(lscape);
	lscape = NULL;
	sharing = false;
}

void lscInit()
{
	static bool oglInited = false;
	extern int texture_extension_number;

	lscCleanup();

	/* Get OpenGL extensions */
	if (!oglInited) {
		ogl_init();
		oglInited = true;
	}

	glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glEnableClientState(GL_VERTEX_ARRAY);

	if (globalGL.numTextureUnits > 1) {
		globalGL.glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		globalGL.glClientActiveTextureARB(GL_TEXTURE1_ARB);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		globalGL.glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		globalGL.glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	else {
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	
	if (lscOccDisable)
		lscOccSectors = lscOccPatchSize = lscOccMaxOccPts = 0;

	/* Create the landscape */
	lscHm = loadHeightmap();
	lscape = lsc_create(lscHmSz, lscPatchSz, lscRepeat, lscHm, lscScale,
						lscTexSize, lscTexTile, lscBaseTexTile,
						lscOccSectors, lscOccPatchSize, lscOccMaxOccPts
						, lscPatchVtxCchSz
						, lscPatchIdxCchSz
						);

	if (lscape->patchVtxCch.fast) {
		globalGL.glVertexArrayRangeNV(
			lscape->patchVtxCch.size, lscape->patchVtxCch.fastBase);
		glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
	}

#ifdef GLQUAKE
	{
		unsigned int i;
		/* Quake doesn't use glGenTextures so make sure
		   we don't grab any of quake's texture names */
		do {
			glGenTextures(1, &i);
		} while (i <= texture_extension_number);
	}
#endif

	/* Create or load textures */
	if (lscape) {
		lsc_createColourMapTexObjs(lscape, lscHm, 1.0f);
		if (globalGL.numTextureUnits > 1)
			lsc_createTextureSplatTexObjs(lscape, lscHm, 1.0f);
	}

	/* Create or load occlusion data */
	if (!lscOccDisable && lscHm && lscape)
		lsc_getOcclusion(lscape, lscHm);

	/* Check extensions */
	if (!globalGL.supportsTextureCombine)
		lscUseTextureCombine = false;
}

void lscShare(lsc l)
{
	lscCleanup();
	lscape = l;
	sharing = true;
}

lsc lscGet(void)
{
	return lscape;
}

bool lscCollisionCheck(vec3 *p, bbox entBox, float *frac, plane collisionPlane)
{
	if (lscCollisionDetect && lscape && lscape->data)
		return lsc_checkCollision(lscape, p, entBox, frac, collisionPlane);
	return false;
}

void lscDraw(cam camera, int win_height)
{
	struct fru_str frustum;

	/* Draw the landscape */
	if (lscape && lscape->data) {

		/* Set up the view frustum */
		fru_init(&frustum, camera);

		lsc_setErrorMetric(lscape, camera->fovY, win_height);

#ifdef TIMER_DETAILED
		QueryPerformanceCounter(&timerDetail[1]);
#endif

		lsc_clip(lscape, &frustum);

#ifdef TIMER_DETAILED
		QueryPerformanceCounter(&timerDetail[2]);
#endif

		lsc_setMipLevel(lscape);

#ifdef TIMER_DETAILED
		QueryPerformanceCounter(&timerDetail[3]);
#endif

		lsc_render(lscape, lscMode, lscOccDisable, lscUseTextureCombine);

#ifdef TIMER_DETAILED
		QueryPerformanceCounter(&timerDetail[4]);
		{
			int i;
			printf("Timers\n");
			for (i = 1; i < 5; i++)
				printf("%d:  %d\n", i,
					timerDetail[i].LowPart - timerDetail[i - 1].LowPart);
			fflush(stdout);
		}
#endif

	}
}

const char* appWindowName()
{
	return "lScape";
}

void appCleanup()
{
	lscCleanup();
	frcam_destroy();
}

void appInit(int argc, char** argv)
{
	cam camera;
	vpt view;
	vec3 origin = {0,0,1000};
	vec3 orientation = {1,0,0};
	vec3 upVector = {0,0,1};
	float aspect;

	if (win_height == 0)
		win_height = 1;
	aspect = (float)win_width / (float)win_height;

	lscInit();

	/* Load the texture mapped font */
	myFont = tmf_create("Data/Font.tga", 14, 27, 0.7f);

	/* Initialise the global camera - this creates it if it doesn't exist */
	camera = cam_getGlobalCamera();
	view = &camera->viewPt;
	vpt_init(view, origin, orientation, upVector);
	camera->fovY = 45.0f;
	cam_setAspect(camera, aspect);
	camera->farPlane = 0.9f * lscHmSz * lscScale[0];
	camera->nearPlane = nearp;

    /* Setup our mover so we can have mouse-driven camera movement */
	cbk_init();
	frcam_create();
	frcam_registerCallbacks();
	frcam_setSpeed(lscHmSz * lscScale[0] / 25.0f);

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, fogMode);
	glFogfv(GL_FOG_COLOR, fogColor);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 0.6f * camera->farPlane);
	glFogf(GL_FOG_END, 0.9f * camera->farPlane);
	glDisable(GL_FOG);
}

void appReInit()
{
	cam camera;
	vpt view;
	vec3 origin = {0,0,1000};
	vec3 orientation = {1,0,0};
	vec3 upVector = {0,0,1};
	float aspect;

	lscCleanup();

	if (win_height == 0)
		win_height = 1;
	aspect = (float)win_width / (float)win_height;

	lscInit();

	/* Load the texture mapped font */
	myFont = tmf_create("Data/Font.tga", 14, 27, 0.7f);

	/* Initialise the global camera - this creates it if it doesn't exist */
	camera = cam_getGlobalCamera();
	view = &camera->viewPt;
	vpt_init(view, origin, orientation, upVector);
	camera->fovY = 45.0f;
	cam_setAspect(camera, aspect);
	camera->farPlane = 0.9f * lscHmSz * lscScale[0];
	camera->nearPlane = nearp;

	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE, fogMode);
	glFogfv(GL_FOG_COLOR, fogColor);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 0.6f * camera->farPlane);
	glFogf(GL_FOG_END, 0.9f * camera->farPlane);
	glDisable(GL_FOG);
}

void appKey(unsigned char key, int x, int y)
{
	cbk_keyboard(key, x, y);

	switch (key) {

	case 'i':
	case 'I':
		lscDrawInfo = !lscDrawInfo;
		break;

	case 'm':
	case 'M':
		lscMode++;
		if (lscMode > 3)
			lscMode = 0;
		if (lscMode > 1 && globalGL.numTextureUnits < 2) {
			lscMode = 0;
			printf("Texture splat requires multitexture extension\n");
			fflush(stdout);
		}
		if (lscMode > 2 && !globalGL.supportsCVA) {
			lscMode = 0;
			printf("CVA not supported");
			fflush(stdout);
		}
		break;

	case 'v':
	case 'V':
		godView = !godView;
		break;

	case 'o':
	case 'O':
		lscOccDisable = !lscOccDisable;
		break;

	case 'c':
	case 'C':
		lscCollisionDetect = !lscCollisionDetect;
		break;

	case 'r':
	case 'R':
		appReInit();
		break;

	case 't':
	case 'T':
		lscUseTextureCombine = !lscUseTextureCombine;
		if (!globalGL.supportsTextureCombine)
			lscUseTextureCombine = false;
		break;

	default:
		break;
	}
}

void appDraw()
{
	cam camera;
	float aspect;
	vpt view;
	vec3 oldOrigin, lookAt;
	vec3 p[2];
	float frac;
	struct plane_str collision;
	struct bbox_str collider = {
		{-16,-16,-24 },
		{ 16, 16, 24 }
	};

	frameCount++;

	if (win_height == 0)
		win_height = 1;
	aspect = (float)win_width / (float)win_height;

	if (!timerBegin) {
		timerBegin = glutGet(GLUT_ELAPSED_TIME);
		timerFrame = timerElapsed = 0;
	}
	else
		timerElapsed = glutGet(GLUT_ELAPSED_TIME) - timerBegin;

#ifdef TIMER_DETAILED
	QueryPerformanceCounter(&timerDetail[0]);
#endif

	/* Get global camera */
	camera = cam_getGlobalCamera();
	cam_setAspect(camera, aspect);
	view = &camera->viewPt;
	vec3_cpy(view->origin, oldOrigin);

	/* Move the global camera using the free camera */
	cbk_update();

	/* Check for collision */
	vec3_cpy(oldOrigin, p[0]);
	vec3_cpy(view->origin, p[1]);
	if (p[0][0] != p[1][0] || p[0][1] != p[1][1] || p[0][2] != p[1][2])
		if (lscCollisionCheck(p, &collider, &frac, &collision))
				vec3_cpy(p[1], view->origin);

	/* Clear Screen & Depth Buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Set up projection & view matrices */
	if (godView) {

		vec3 orientation = {0,0,-1};
		vec3 origin = {0,0,15000};
		vec3 upVector = {0,0,1};
		cam godCam = cam_create();

		vec3_add(view->origin, origin, origin);
		vec3_cpy(view->orientation, orientation);
		orientation[2] = -1;
		origin[0] -= orientation[0] * 1000;
		origin[1] -= orientation[1] * 1000;
		view = &godCam->viewPt;
		vpt_init(view, origin, orientation, upVector);
		godCam->fovY = 45.0f;
		cam_setAspect(godCam, aspect);
		godCam->farPlane = 100 * lscHmSz * lscScale[0];
		godCam->nearPlane = nearp;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(godCam->fovY, godCam->aspect,
						godCam->nearPlane, godCam->farPlane);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		vec3_add(view->origin, view->orientation, lookAt);
		gluLookAt(view->origin[0], view->origin[1], view->origin[2],
				  lookAt[0], lookAt[1], lookAt[2],
				  view->upVector[0], view->upVector[1], view->upVector[2]);

		cam_destroy(godCam);
	}
	else {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(camera->fovY, camera->aspect,
						camera->nearPlane, camera->farPlane);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		vec3_add(view->origin, view->orientation, lookAt);
		gluLookAt(view->origin[0], view->origin[1], view->origin[2],
				  lookAt[0], lookAt[1], lookAt[2],
				  view->upVector[0], view->upVector[1], view->upVector[2]);

		glEnable(GL_FOG);
	}

	/* Draw the landscape */
	lscDraw(camera, win_height);

	glDisable(GL_FOG);
	drawInfo();
}
