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

	Function:	glBase simple test app
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
#include "pbuffer.h"

#define WATER_COLS		10
#define WATER_ROWS		10
#define WATER_STEP		0.5f
#define WATER_TEXW		(1.0f/WATER_COLS)
#define WATER_TEXH		(1.0f/WATER_ROWS)

/*
 * Note for 3dfx cards - 256 x 256 max, but the
 * resulting reflections look blocky.
 */
#define REFL_TEX	4			/* Index of reflection texture */
#define REFL_TEXW	512			/* Reflection texture width */
#define REFL_TEXH	512			/* Reflection texture height */

/*
 * Timer for water ripple animation and fps
 */
static int timerBegin;
static int timerElapsed;
static int timerFrame;
static int frameCount;

/*
 * Array for all texture object ids
 */
static int texture[5];

/*
 * Dimensions of current usable area of reflection texture
 */
int reflTexW = 0;
int reflTexH = 0;

/*
 * Quadratic for drawing the ball
 */
static GLUquadricObj *sphere;
static vec3 spherePos = { 0.0f, 0.3f, 0.0f };
static float sphereRadius = 0.35f;

/*
 * Lighting
 */
/* White ambient light at half intensity (rgba) */
static float lightAmb[] = { 0.5f, 0.5f, 0.5f, 1.0f };

/* Super bright, full intensity diffuse light */
static float lightDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };

/* Position of light */
static float lightPos[] = { 0.0f, 0.0f, 2.0f, 1.0f };

/*
 * Number of objects in current scene
 */
static int countObjects;

/*
 * Use render to texture to update the reflection?
 */
static bool updtRefl = true;

/*
 * Texture mapped font
 */
static tmf myFont;

/*
 * Pixel buffer for off-screen rendering
 */
static pbf pbuffer;

/*
 * Use pbuffer instead of frame buffer?
 */
static bool usePbuf = true;

/*
 * Use make_current_read_ARB?
 */
static bool useMakeRead = false;



void loadTextures()
{
	unsigned char *data;

	texture[0] = txm_addTgaFile("Data/Water.tga", false, true, true);
	texture[1] = txm_addTgaFile("Data/Ball.tga", false, true, true);
	texture[2] = txm_addTgaFile("Data/Envmap.tga", false, true, true);
	texture[3] = txm_addTgaFile("Data/Mountains.tga", false, true, true);

	/* Create an empty texture for reflections */
	data = (unsigned char *)calloc(3*REFL_TEXW*REFL_TEXH, sizeof(unsigned char));
	texture[REFL_TEX] = txm_addRawData(data, REFL_TEXW, REFL_TEXH, GL_RGB, false, false);

	/* Load the texture mapped font */
	myFont = tmf_create("Data/Font.tga", 14, 27, 0.7f);
}

void drawBall()
{
	glEnable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTranslatef(spherePos[0], spherePos[1], spherePos[2]);
	gluSphere(sphere, sphereRadius, 32, 16);
	glTranslatef(-spherePos[0], -spherePos[1], -spherePos[2]);
	glDisable(GL_LIGHTING);
	countObjects++;
}

bool isBallVisible(fru f)
{
	struct bbox_str b;
	int clipFlags = 126;		/* All on */
	vec3_subS(spherePos, sphereRadius, b.minp);
	vec3_addS(spherePos, sphereRadius, b.maxp);
	clipFlags = bbox_isInsideFrustum(&b, f, clipFlags);
	return (clipFlags != 1);
}

void drawBackdrop()
{
	float x = WATER_COLS * WATER_STEP / 2.0f;
	float y = WATER_ROWS * WATER_STEP;
	float z = WATER_ROWS * WATER_STEP / 2.0f;

	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, 0.0f, -z);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-x, y, -z);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f( x, y, -z);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f( x, 0.0f, -z);
	glEnd();
	countObjects++;
}

void drawWater()
{
	int i, j;
	float ofsx = WATER_COLS * WATER_STEP / 2.0f;
	float ofsz = WATER_ROWS * WATER_STEP / 2.0f;
	float ofsi = 0.02 * sin(timerElapsed / 1000.0);
	float ofsj = 0.02 * cos(timerElapsed / 1000.0);

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	for (j = 0; j < WATER_ROWS; j++) {
		glBegin(GL_TRIANGLE_STRIP);
		for (i = 0; i <= WATER_COLS; i++) {
			glTexCoord2f(i*WATER_TEXW, j*WATER_TEXH);
			glVertex3f(i*WATER_STEP - ofsx,
						((i%3)-1)*ofsi + ((j%3)-1)*ofsj,
						j*WATER_STEP - ofsz);
			glTexCoord2f(i*WATER_TEXW, (j+1)*WATER_TEXH);
			glVertex3f(i*WATER_STEP - ofsx,
						((i%3)-1)*ofsi + (((j+1)%3)-1)*ofsj,
						(j+1)*WATER_STEP - ofsz);
		}
		glEnd();
	}
	countObjects++;
}

bool isMainVisible(fru f)
{
	struct bbox_str b = { { -WATER_COLS * WATER_STEP / 2.0f,
						    0.0f,
						    -WATER_ROWS * WATER_STEP / 2.0f },
						  {  WATER_COLS * WATER_STEP / 2.0f,
						     WATER_ROWS * WATER_STEP,
							 WATER_ROWS * WATER_STEP / 2.0f } };
	int clipFlags = 126;		/* All on */
	clipFlags = bbox_isInsideFrustum(&b, f, clipFlags);
	return (clipFlags != 1);
}

/*
 * Draw reflected scene into reflection texture
 */
void updateReflectionTexture(cam c)
{
	vpt view;
	vec3 lookAt;
	float fov = (c->fovX > c->fovY) ? c->fovX : c->fovY;
	float aspect;
	double clipPlane[] = { 0, 1, 0, 0 };

	if (usePbuf && pbuffer && pbuffer->isValid) {
		pbf_makeCurrent(pbuffer);
		reflTexW = (pbuffer->width < REFL_TEXW) ? pbuffer->width : REFL_TEXW;
		reflTexH = (pbuffer->height < REFL_TEXH) ? pbuffer->height : REFL_TEXH;
	}
	else {
		reflTexW = (win_width < REFL_TEXW) ? win_width : REFL_TEXW;
		reflTexH = (win_height < REFL_TEXH) ? win_height : REFL_TEXH;
	}
	aspect = (float)reflTexW / (float)reflTexH;

	glClearColor(0.2f, 0.5f, 1.0f, 1.0f);		/* Background */

	/* Set the viewport to match the size of the reflection texture */
    glViewport(0, 0, reflTexW, reflTexH);

	/* Clear Screen & Depth Buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Set the projection matrix */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, c->nearPlane, c->farPlane);

	/*
	 * Set the modelview matrix, flip everything about the reflection plane.
	 */
	view = &c->viewPt;
	vec3_add(view->origin, view->orientation, lookAt);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(view->origin[0], -view->origin[1], view->origin[2],
			  lookAt[0], -lookAt[1], lookAt[2],
			  view->upVector[0], -view->upVector[1], view->upVector[2]);

	/*
	 * Set up a clip plane so that we don't get a reflection of
	 * the ball when it is under the water.
	 */
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, clipPlane);

	drawBackdrop();
	drawBall();

	glDisable(GL_CLIP_PLANE0);

	if (usePbuf && useMakeRead && pbuffer && pbuffer->isValid)
		pbf_makeRead(pbuffer);

	/* Copy scene into reflection texture */
	if (updtRefl) {
		glBindTexture(GL_TEXTURE_2D, texture[REFL_TEX]);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
							(REFL_TEXW - reflTexW) >> 1,
							(REFL_TEXH - reflTexH) >> 1,
							0, 0, reflTexW, reflTexH);
		/* glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, REFL_TEXW, REFL_TEXH, 0); */
	}

	if (usePbuf && pbuffer && pbuffer->isValid)
		pbf_makeGlutWindowCurrent(pbuffer);

	/* Reset the viewport */
	glViewport(0, 0, win_width, win_height);
}

/*
 * Load projection into texture matrix
 */
void loadProjectionMatrix(cam c)
{
	vpt view;
	vec3 lookAt;
	float fov = (c->fovX > c->fovY) ? c->fovX : c->fovY;
	float aspect = (float)reflTexW / (float)reflTexH;

	view = &c->viewPt;
	vec3_add(view->origin, view->orientation, lookAt);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glTranslatef(0.5, 0.5, 0.0);				/* Center texture */
	glScalef(0.5f * (float)reflTexW / REFL_TEXW,
			 0.5f * (float)reflTexH / REFL_TEXH,
			 1.0);								/* Scale and bias */
	gluPerspective(fov, aspect, c->nearPlane, c->farPlane);
	gluLookAt(view->origin[0], -view->origin[1], view->origin[2],
			  lookAt[0], -lookAt[1], lookAt[2],
			  view->upVector[0], -view->upVector[1], view->upVector[2]);
	glMatrixMode(GL_MODELVIEW);
}

/*
 * Load identity into texture matrix
 */
void clearProjectionMatrix()
{
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}

/*
 * Draw the reflection
 */
void drawReflection(cam c)
{
	int i, j;
	float ofsx = WATER_COLS * WATER_STEP / 2.0f;
	float ofsz = WATER_ROWS * WATER_STEP / 2.0f;
	float ofsi = 0.02 * sin(timerElapsed / 1000.0);
	float ofsj = 0.02 * cos(timerElapsed / 1000.0);

	/*
	 * On the Radeon, and maybe on other T&L cards, blending a projected
	 * texture on top of a non-projected texture causes z-fighting, so I
	 * use delta to raise the reflection slightly above the water.
	 */
	float delta = 0.01f;

	glEnable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	 /* Blending Based On Source Alpha And 1 Minus Dest Alpha */
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	loadProjectionMatrix(c);
	glBindTexture(GL_TEXTURE_2D, texture[REFL_TEX]);
	for (j = 0; j < WATER_ROWS; j++) {
		glBegin(GL_TRIANGLE_STRIP);
		for (i = 0; i <= WATER_COLS; i++) {
			glTexCoord3f(i*WATER_STEP - ofsx - ((j%2)-0.5f)*ofsi,
							0.0f,
							j*WATER_STEP - ofsz - ((i%2)-0.5f)*ofsj);
			glVertex3f(i*WATER_STEP - ofsx,
							((i%3)-1)*ofsi + ((j%3)-1)*ofsj + delta,
							j*WATER_STEP - ofsz);
			glTexCoord3f(i*WATER_STEP - ofsx - (((j+1)%2)-0.5f)*ofsi,
							0.0f,
							(j+1)*WATER_STEP - ofsz - ((i%2)-0.5f)*ofsj);
			glVertex3f(i*WATER_STEP - ofsx,
							((i%3)-1)*ofsi + (((j+1)%3)-1)*ofsj + delta,
							(j+1)*WATER_STEP - ofsz);
		}
		glEnd();
	}
	clearProjectionMatrix();
	glDisable(GL_BLEND);
	countObjects++;
}

/*
 * Draw any messages in the top left of the window
 */
void drawInfo(void)
{
	static char msgbf[][64] = {	"xxxx objects drawn",
								"xxxx fps",
								"reflection update xxxx",
								"using pbuffer and make current read",
								"window size xxxx x xxxx",
								"reflection texture size xxxx x xxxx"	};
	static int fps = 0;

	char *msg[7];
	int i;

	for (i = 0; i< 6; i++)
		msg[i] = msgbf[i];
	msg[i] = NULL;

	if ((timerElapsed - timerFrame) > 1000) {
		fps = (int)((1000.0f * frameCount) / (float)(timerElapsed - timerFrame));
		timerFrame = timerElapsed;
		frameCount = 0;
	}

	if (myFont) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, win_width, win_height, 0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		sprintf(msg[0], "%d objects drawn", countObjects);
		sprintf(msg[1], "%d fps", fps);
		sprintf(msg[2], "reflection update %s", updtRefl ? "on" : "off");
		if (usePbuf && pbuffer && pbuffer->isValid) {
			if (useMakeRead)
				sprintf(msg[3], "using pbuffer and make current read");
			else
				sprintf(msg[3], "using pbuffer");
		}
		else
			sprintf(msg[3], "using back buffer");
		sprintf(msg[4], "window size %d x %d", win_width, win_height);
		sprintf(msg[5], "reflection texture size %d x %d", reflTexW, reflTexH);
		tmf_drawStrings(myFont, 0, 0, msg, 1.0f);
	}
}

const char* appWindowName()
{
	return "glBase test app";
}

void appCleanup()
{
	txm_deleteTextures();
	cam_destroy(cam_getGlobalCamera());
	frcam_destroy();
}

void appInit(int argc, char** argv)
{
	cam camera;
	vpt view;
	vec3 origin = {0,1,7};
	vec3 orientation = {0,0,-1};
	vec3 upVector = {0,1,0};
	float aspect;

	if (win_height == 0)
		win_height = 1;
	aspect = (float)win_width / (float)win_height;

	/* Get OpenGL extensions */
	ogl_init();

	/* Initialise the global camera - this creates it if it doesn't exist */
	camera = cam_getGlobalCamera();
	view = &camera->viewPt;
	vpt_init(view, origin, orientation, upVector);
	camera->fovY = 45.0f;
	cam_setAspect(camera, aspect);
	camera->farPlane = 100.0f;
	camera->nearPlane = 0.1f;

    /* Setup our mover so we can have mouse-driven camera movement */
	cbk_init();
	frcam_create();
	frcam_registerCallbacks();
	frcam_setSpeed(1.0f);

	loadTextures();

	glShadeModel(GL_SMOOTH);
	glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_TEXTURE_2D);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	sphere = gluNewQuadric();
	gluQuadricNormals(sphere, GL_SMOOTH);
	gluQuadricTexture(sphere, GL_TRUE);

	/* If we support it, get a pbuffer */
	pbuffer = NULL;
	if (globalGL.supportsPBuffer)
		pbuffer = pbf_create(REFL_TEXW, REFL_TEXH,
						GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH, true);
	if (pbuffer && pbuffer->isValid) {
		pbf_makeCurrent(pbuffer);
		glShadeModel(GL_SMOOTH);
		glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
		pbf_makeGlutWindowCurrent(pbuffer);
	}
}

void appKey(unsigned char key, int x, int y)
{
	cbk_keyboard(key, x, y);

	switch (key) {
	case 'u':
	case 'U':
		spherePos[1] += 0.1f;
		break;
	case 'd':
	case 'D':
		spherePos[1] -= 0.1f;
		break;
	case 'r':
	case 'R':
		updtRefl = !updtRefl;
		break;
	case 'p':
	case 'P':
		usePbuf = !usePbuf;
		useMakeRead = false;
		break;
	case 'm':
	case 'M':
		if (usePbuf)
			useMakeRead = !useMakeRead;
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
	vec3 lookAt;
	struct fru_str frustum;
	int visible;

	countObjects = 0;
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

	/* Move the global camera using the free camera */
	cbk_update();

	/* Get camera */
	camera = cam_getGlobalCamera();
	cam_setAspect(camera, aspect);
	view = &camera->viewPt;
	vec3_add(view->origin, view->orientation, lookAt);

	/* Set up the view frustum */
	fru_init(&frustum, camera);

	/* Check objects against frustum */
	visible = 0;
	if (isMainVisible(&frustum))
		visible |= 1;
	if (isBallVisible(&frustum))
		visible |= 2;
	if (!visible) {
		glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		drawInfo();
		return;
	}

	/* Refresh reflection texture */
	updateReflectionTexture(camera);

	/* Clear Screen & Depth Buffer */
	glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Set up projection matrix */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera->fovY, camera->aspect,
					camera->nearPlane, camera->farPlane);

	/* Set up view matrix */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(view->origin[0], view->origin[1], view->origin[2],
			  lookAt[0], lookAt[1], lookAt[2],
			  view->upVector[0], view->upVector[1], view->upVector[2]);

	/* Draw scene */
	if (visible & 1) {
		drawBackdrop();
		drawWater();
		drawReflection(camera);
	}
	if (visible & 2)
		drawBall();
	drawInfo();
}
