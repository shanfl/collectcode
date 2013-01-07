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
	File:		textureManager.h

	Function:	Manage loading textures into OpenGL texture units.
*/

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "general.h"
#include "opengl.h"

int txm_addRawFile(char *filename, int w, int h, int format, bool repeat, bool mipmap);
int txm_addTgaFile(char *filename, bool alpha, bool repeat, bool mipmap);
int txm_addRawData(unsigned char* data, int w, int h, int format, bool repeat, bool mipmap);
void txm_deleteTextures();

#endif /* TEXTUREMANAGER_H */
