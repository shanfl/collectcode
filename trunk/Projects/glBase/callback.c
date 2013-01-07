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
	File:		callback.c

	Function:	callback class, provides a set of functions that
				will be used as callback functions by GLUT.
*/

#include <gl/glut.h>
#include "callback.h"

static cbk callback;

static void cbk_mouse(int button, int state, int x, int y)
{
	if (callback)
		callback->mouse(callback, button, state, x, y);
}

static void cbk_motion(int x, int y)
{
	if (callback)
		callback->motion(callback, x,y);
}

static void cbk_special(int key, int x, int y)
{
	if (callback)
		callback->special(callback, key, x, y);
}

void cbk_update(void)
{
	if (callback)
		callback->update(callback);
}

void cbk_keyboard(unsigned char key, int x, int y)
{
	if (callback)
		callback->keyboard(callback, key, x, y);
}

void cbk_init(void)
{
	glutMouseFunc(cbk_mouse);
	glutMotionFunc(cbk_motion);
	glutPassiveMotionFunc(cbk_motion);
	glutSpecialFunc(cbk_special);

	/*
	 * cbk_update() and cbk_keyboard() are not registered directly with
	 * GLUT but are called from our own appKey() and appDraw() routines
	 * (which are registered with GLUT)
	 */
}

void cbk_register(cbk c)
{
	callback = c;
}
