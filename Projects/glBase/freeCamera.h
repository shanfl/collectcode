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
	File:		freecamera.h

	Function:	free camera class, provides a set of callback functions
				that provide a simple no collision detection camera mover
				that allows the user to fly around in 3D using the mouse.

				This is a singleton class.

				Use mouseLeft to move forward
				mouseMid / mouseRight to move backwards
				mouse to look / change direction
*/

#ifndef FREECAMERA_H
#define FREECAMERA_H

#include "camera.h"
#include "callback.h"

bool frcam_create(void);
void frcam_destroy(void);
bool frcam_registerCallbacks(void);
void frcam_setSpeed(float speed);

#endif /* FREECAMERA_H */
