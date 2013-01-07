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
	File:		freecamera.c

	Function:	free camera class, provides a set of callback functions
				that provide a simple no collision detection camera mover
				that allows the user to fly around in 3D using the mouse.

				This is a singleton class.

				Use mouseLeft to move forward
				mouseMid / mouseRight to move backwards
				mouse to look / change direction
*/

#include <gl/glut.h>
#include "camera.h"
#include "freecamera.h"

typedef struct frcam_str
{
	struct cbk_str callback;
	float speed;
	bool mouseDown;
	int  mouseButton;
	long lastButtonDownTime;
	bool haveMoved;
	long locationLastTime;
	long lastFrameTime;
	int  cenX, cenY;
	float accumX, accumY;
	vec3 upVector;

	/* TODO: Debug */
	bool leftButton;
	bool upButton;

} *frcam;

static frcam freeCamera;

static void frcam_mouse(cbk c, int button, int state, int x, int y)
{
	frcam f = (frcam)c;
	f->mouseDown = (state == GLUT_DOWN) ? true : false;
	f->mouseButton = button;
	f->lastButtonDownTime = glutGet(GLUT_ELAPSED_TIME);
}

static void frcam_motion(cbk c, int x, int y)
{
	float dx, dy;
	frcam f = (frcam)c;

	if (!f->haveMoved) {
		f->cenX = glutGet(GLUT_WINDOW_X) + glutGet(GLUT_WINDOW_WIDTH)/2;
		f->cenY = glutGet(GLUT_WINDOW_Y) + glutGet(GLUT_WINDOW_HEIGHT)/2;
		glutWarpPointer(f->cenX, f->cenY);
		f->accumX = 0;
		f->accumY = 0;
		f->haveMoved = true;
	}
	else {
		if (f->cenX == x && f->cenY == y)
			return;
		dx = f->cenX - x;
		dy = f->cenY - y;
		f->accumX += dx;
		f->accumY += dy;
		glutWarpPointer(f->cenX, f->cenY);
	}
}

static void frcam_special(cbk c, int key, int x, int y)
{
}

static void frcam_update(cbk c)
{
	bool button1, button2;
	float dt, direction;
	long curTime = glutGet(GLUT_ELAPSED_TIME);
	frcam f = (frcam)c;
	vpt view = &cam_getGlobalCamera()->viewPt;

	if (f->lastFrameTime == 0) {
		f->lastFrameTime = curTime;
		return;
	}

	dt = (curTime - f->lastFrameTime) / 1000.0f;

	/*
	 * Go forward if the left button is down, backwards
	 * if any other mouse button is down.
	 */
	direction = (f->mouseButton == GLUT_LEFT_BUTTON) ? 1.0f : -1.0f;

	button1 = f->mouseDown && (f->mouseButton == GLUT_LEFT_BUTTON);
	button2 = f->mouseDown && (f->mouseButton != GLUT_LEFT_BUTTON);

	vpt_pitch(view, 0.004f * f->accumY);
	vpt_rotate(view, f->upVector, 0.004f * f->accumX);

	/* Reset these since we've taken them into account now. */
	f->accumX = 0;
	f->accumY = 0;

	/* TODO: Debug */
/*	if (f->mouseDown)
		vpt_translateBy(view, view->orientation, direction * dt * f->speed); */
	if (f->mouseDown) {
		if (f->leftButton) {
			vpt_translateBy(view, view->leftVector, direction * dt * f->speed);
		}
		else if (f->upButton) {
			vpt_translateBy(view, view->upVector, direction * dt * f->speed);
		}
		else {
			vpt_translateBy(view, view->orientation, direction * dt * f->speed);
		}
	}

	f->lastFrameTime = curTime;
}

static void frcam_keyboard(cbk c, unsigned char key, int x, int y)
{
	/* TODO: Debug */
	frcam f = (frcam)c;
	switch (key) {
	case 'u':
	case 'U':
		f->upButton = !f->upButton;
		break;

	case 'l':
	case 'L':
		f->leftButton = !f->leftButton;
		break;
	}
}

static void frcam_default(frcam f)
{
	cam c;
	vpt v;

	if (f) {
		f->callback.mouse = 	frcam_mouse;
		f->callback.motion = 	frcam_motion;
		f->callback.special = 	frcam_special;
		f->callback.update = 	frcam_update;
		f->callback.keyboard = 	frcam_keyboard;

		f->speed = 10.0f;
		f->mouseDown = false;
		f->mouseButton = GLUT_LEFT_BUTTON;
		f->lastButtonDownTime = 0;
		f->haveMoved = false;
		f->locationLastTime = 0;
		f->lastFrameTime = 0;
		f->cenX = 0;
		f->cenY = 0;
		f->accumX = 0;
		f->accumY = 0;

		/* TODO: Debug */
		f->leftButton = false;
		f->upButton = false;

		c = cam_getGlobalCamera();
		v = &c->viewPt;
		vec3_cpy(v->upVector, f->upVector);
	}
}

bool frcam_create(void)
{
	if (freeCamera)
		return true;
	freeCamera = (frcam)malloc(sizeof(struct frcam_str));
	if (freeCamera)
		frcam_default(freeCamera);
	return (freeCamera != NULL);
}

void frcam_destroy(void)
{
	if (freeCamera)
		free(freeCamera);
}

bool frcam_registerCallbacks(void)
{
	if (!freeCamera)
		return false;
	cbk_register(&freeCamera->callback);
	return true;
}

void frcam_setSpeed(float speed)
{
	if (freeCamera)
		freeCamera->speed = speed;
}
