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
	File:		textureFont.h

	Function:	Class for drawing texture mapped fonts.
*/

#ifndef TEXTUREFONT_H
#define TEXTUREFONT_H

#include "pic.h"

typedef struct tmf_str {
  struct pic_str p;
  int fontWidth;
  int fontHeight;
  int texWidth;
  int texHeight;
} *tmf;

tmf tmf_create(char *tgaFilename, int fWidth, int fHeight, float fScale);
void tmf_default(tmf t, char *tgaFilename, int fWidth, int fHeight, float fScale);
void tmf_destroy(tmf t);
void tmf_drawString(tmf t, int x, int y, char *str, float alpha);
void tmf_drawStrings(tmf t, int x, int y, char **str, float alpha);

#endif /* TEXTUREFONT_H */
