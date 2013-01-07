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
	File:		pic.h

	Function:	Pic class for drwing 2D tga pictures and texture mapped
				fonts.
*/

#ifndef PIC_H
#define PIC_H

#include "general.h"

typedef struct pic_str {
	int texnum;
	int width;
	int height;
} *pic;

pic pic_create(char *tgaFilename);
pic pic_createAlpha(char *tgaFilename, unsigned char *alphaColour);
void pic_default(pic p, char *tgaFilename);
void pic_defaultAlpha(pic p, char *tgaFilename, unsigned char *alphaColour);
void pic_destroy(pic p);
void pic_draw(pic p, int x, int y);
void pic_drawCharacter(pic p, int x, int y, int num, int w, int h, int tw, int th);
void pic_drawString(pic p, int x, int y, char *str, int w, int h, int tw, int th);

#endif /* PIC_H */
